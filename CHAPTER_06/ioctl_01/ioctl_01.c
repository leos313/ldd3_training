/*
 *
 * Author  :   Leonardo Suriano <leonardo.suriano@live.it>
 * Date    :   January 2018
 *
 * Description: TODO
 *
 * */

 #include <linux/init.h>          /* needed for module_init and exit */
 #include <linux/module.h>
 #include <linux/moduleparam.h>   /* needed for module_param */
 #include <linux/kernel.h>        /* needed for printk */
 #include <linux/sched.h>         /* needed for current-> */
 #include <linux/types.h>         /* needed for dev_t type */
 #include <linux/kdev_t.h>        /* needed for macros MAJOR, MINOR, MKDEV... */
 #include <linux/fs.h>            /* needed for register_chrdev_region, file_operations */
 #include <linux/cdev.h>          /* cdev definition */
 #include <linux/slab.h>		       /* kmalloc(),kfree() */
 #include <asm/uaccess.h>         /* copy_to copy_from _user */

 #include "ioctl_01.h"

 int ioctl_01_major = 0;
 int ioctl_01_minor = 0;
 unsigned int ioctl_01_nr_devs = 1;
 int device_max_size = DEVICE_MAX_SIZE;
 unsigned long long read_times = 0;
 unsigned long long write_times = 0;

 module_param(ioctl_01_major, int, S_IRUGO);
 module_param(ioctl_01_minor, int, S_IRUGO);
 //module_param(ioctl_01_nr_devs, int, S_IRUGO);
 module_param(device_max_size, int, S_IRUGO);

 struct ioctl_01_dev *ioctl_01_devices;	/* allocated in ioctl_01_init_module */

 /*
  * Open and close (close = release)
  */

 int ioctl_01_open(struct inode *inode, struct file *filp)
 {
     struct ioctl_01_dev *dev; /* device information */
     PDEBUG(" performing 'open' operation\n");
     dev = container_of(inode->i_cdev, struct ioctl_01_dev, cdev);
     filp->private_data = dev; /* for other methods */

 	  return 0;          /* success */
 }

 int ioctl_01_release(struct inode *inode, struct file *filp)
 {
     PDEBUG(" performing 'release' operation\n");
     return 0;
 }

 /*
  * Data management: read and write
  */

 ssize_t ioctl_01_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
 {
     //struct ioctl_01_dev *dev = filp->private_data;
     ssize_t retval = 0;
     if (count > device_max_size){
         printk(KERN_WARNING "[LEO] ioctl_01: trying to read more than possible. Aborting read\n");
         retval = -EFBIG;
         goto out;
     }
     if (down_interruptible(&(ioctl_01_devices->sem_ioctl_01))){
         printk(KERN_WARNING "[LEO] ioctl_01: Device was busy. Operation aborted\n");
         return -ERESTARTSYS;
     }
     if (copy_to_user(buf, (void*)ioctl_01_devices -> p_data, count)) {
         printk(KERN_WARNING "[LEO] ioctl_01: can't use copy_to_user. \n");
 		    retval = -EPERM;
 		    goto out_and_Vsem;
 	  }

     out_and_Vsem:
     read_times++;
     up(&(ioctl_01_devices->sem_ioctl_01));
     out:
     return retval;
 }

 ssize_t ioctl_01_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
 {
     int retval = 0;
     //struct ioctl_01_dev *dev = filp->private_data;
     if (count > device_max_size){
         printk(KERN_WARNING "[LEO] ioctl_01: trying to write more than possible. Aborting write\n");
         retval = -EFBIG;
         goto out;
     }
     if (down_interruptible(&(ioctl_01_devices->sem_ioctl_01))){
         printk(KERN_WARNING "[LEO] ioctl_01: Device was busy. Operation aborted\n");
         return -ERESTARTSYS;
     }
     if (copy_from_user((void*)ioctl_01_devices -> p_data, buf, count)) {
         printk(KERN_WARNING "[LEO] ioctl_01: can't use copy_from_user. \n");
         retval = -EPERM;
         goto out_and_Vsem;
     }

     out_and_Vsem:
     write_times++;
     up(&(ioctl_01_devices->sem_ioctl_01));
     out:
     return retval;
 }


 /*
  * Create a set of file operations for our ioctl_01 files.
  * All the functions do nothig
  */

 struct file_operations ioctl_01_fops = {
     .owner =    THIS_MODULE,
     .read =     ioctl_01_read,
     .write =    ioctl_01_write,
     .open =     ioctl_01_open,
     .release =  ioctl_01_release,
 };

 /*
  * The cleanup function is used to handle initialization failures as well.
  * Thefore, it must be careful to work correctly even if some of the items
  * have not been initialized
  */

  void ioctl_01_cleanup_module(void)
 {
 	  dev_t devno = MKDEV(ioctl_01_major, ioctl_01_minor);
     cdev_del(&(ioctl_01_devices->cdev));
     /* freeing the memory */
     if((ioctl_01_devices -> p_data) != 0){
         kfree(ioctl_01_devices -> p_data);
         PDEBUG(" kfree the string-memory\n");
     }
     if((ioctl_01_devices) != 0){
         kfree(ioctl_01_devices);
         PDEBUG(" kfree ioctl_01_devices\n");
     }
 	  unregister_chrdev_region(devno, ioctl_01_nr_devs);        /* unregistering device */
 		PDEBUG(" cdev deleted, kfree, chdev unregistered\n");
 }

 /*
  * Set up the char_dev structure for this device.
  */

 static void ioctl_01_setup_cdev(struct ioctl_01_dev *dev)
 {
 	int err, devno = MKDEV(ioctl_01_major, ioctl_01_minor);

 	cdev_init(&dev->cdev, &ioctl_01_fops);
 	dev->cdev.owner = THIS_MODULE;
 	dev->cdev.ops = &ioctl_01_fops;
 	err = cdev_add (&dev->cdev, devno, 1);
 	/* Fail gracefully if need be */
 	if (err)
 		printk(KERN_WARNING "[LEO] Error %d adding ioctl_01 cdev_add", err);

   PDEBUG(" cdev initialized\n");
 }

 /*
  * The init function is used to register the chdeiv allocating dinamically a
  * new major number (if not specified at load/compilation-time)
  */

 static int ioctl_01_init(void)
 {
 	  int result =0;
 	  dev_t dev = 0;

 	  if (ioctl_01_major) {
 			  PDEBUG(" static allocation of major number (%d)\n",ioctl_01_major);
 		    dev = MKDEV(ioctl_01_major, ioctl_01_minor);
 		    result = register_chrdev_region(dev, ioctl_01_nr_devs, "ioctl_01");
 	  } else {
 			  PDEBUG("ioctl_01: dinamic allocation of major number\n");
 		    result = alloc_chrdev_region(&dev, ioctl_01_minor, ioctl_01_nr_devs, "ioctl_01");
 		    ioctl_01_major = MAJOR(dev);
 	  }
 	  if (result < 0) {
 		    printk(KERN_WARNING "[LEO] ioctl_01: can't get major %d\n", ioctl_01_major);
 		    return result;
     }

     ioctl_01_devices = kmalloc(ioctl_01_nr_devs * sizeof(struct ioctl_01_dev), GFP_KERNEL);
     if (!ioctl_01_devices) {
         result = -ENOMEM;
         printk(KERN_WARNING "[LEO] ioctl_01: ERROR kmalloc dev struct\n");
         goto fail;  /* Make this more graceful */
     }

     memset(ioctl_01_devices, 0, ioctl_01_nr_devs * sizeof(struct ioctl_01_dev));
     /* Initialize the device. */

     ioctl_01_devices -> p_data = (char*)kmalloc(device_max_size * sizeof(char), GFP_KERNEL);
     if (!ioctl_01_devices -> p_data) {
         result = -ENOMEM;
         printk(KERN_WARNING "[LEO] ioctl_01: ERROR kmalloc p_data\n");
         goto fail;  /* Make this more graceful */
     }
     sema_init(&(ioctl_01_devices->sem_ioctl_01), 1); /* semaphore initialization */
     ioctl_01_setup_cdev(ioctl_01_devices);

     return 0;

     fail:
     ioctl_01_cleanup_module();
     return result;
 }

 /*
  * The exit function is simply calls the cleanup
  */

 static void ioctl_01_exit(void)
 {
 	  PDEBUG(" --calling cleanup function--\n");
 	  ioctl_01_cleanup_module();
 }

 module_init(ioctl_01_init);
 module_exit(ioctl_01_exit);

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
 MODULE_DESCRIPTION("example of using ioctl(...)");
 MODULE_VERSION("1.0");
