/*
 *
 * Author  :   Leonardo Suriano <leonardo.suriano@live.it>
 * Date    :   January 2018
 *
 * Description: TODO
 *
 * BE CAREFULL TO SET THE DIRECTON:
 * #define XGPIO_DATA_OFFSET	0x0   < Data register for 1st channel
 * #define XGPIO_TRI_OFFSET	0x4   < I/O direction reg for 1st channel
 * #define XGPIO_DATA2_OFFSET	0x8   < Data register for 2nd channel
 * #define XGPIO_TRI2_OFFSET	0xC   < I/O direction reg for 2nd channel
 *
 * #define XGPIO_GIE_OFFSET	0x11C < Glogal interrupt enable register
 * #define XGPIO_ISR_OFFSET	0x120 < Interrupt status register
 * #define XGPIO_IER_OFFSET	0x128 < Interrupt enable register
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
 #include <linux/slab.h>		      /* kmalloc(),kfree() */
 #include <asm/uaccess.h>         /* copy_to copy_from _user */
 #include <linux/of.h>
 #include <linux/of_address.h>
 #include <linux/of_device.h>
 #include <linux/of_platform.h>
 #include <asm/io.h>               /* iowrite32() and company */
 #include <linux/ioport.h>         /* I/O port allocation request_resource(...) */

 #include "LED_01.h"

 int LED_01_major = 0;
 int LED_01_minor = 0;
 unsigned int LED_01_nr_devs = 1;
 unsigned long long read_times = 0;
 unsigned long long write_times = 0;
 u32 LED_value = 7;

 struct LED_01_dev *LED_01_devices;	/* allocated in LED_01_init_module */

 /* PLATFORM DRIVER */

 /* Match table for of_platform binding */
 static struct of_device_id LED_of_match[] = {
  { .compatible = "xlnx,xps-gpio-1.00.a,generic-uio,LED",
    //.compatible = "xlnx,xps-gpio-1.00.a,generic-uio,BTandSW",
    //.compatible = "xlnx,xps-gpio-1.00.a,generic-uio,LED",
   },
  {}
 };

 MODULE_DEVICE_TABLE(of, LED_of_match);

 static struct resource res;
 unsigned long *base_addr; /* Virtual Base Address */
 /* platform device structures */


 static int LED_of_probe(struct platform_device *op)
 {
     int ret;

     ret = of_address_to_resource(op->dev.of_node, 0, &res);
     if (ret) {
         printk(KERN_WARNING "[LEO] LED: Failed to obtain device tree resource\n");
         return ret;
     }

     PDEBUG("LED: Physical address to resource is %x\n", (unsigned int) res.start);

    //  //platform_get_resource(op, IORESOURCE_MEM, 0);
    //  //request_mem_region(res.start, resource_size(led_brightness_dev->mem), pdev->name);
    //  base_addr = ioremap(res.start, 0x10000);
    //  __raw_writel(1,base_addr);
    struct resource* mem_region_requested = request_mem_region(res.start,0x10000,"LED_01");
    if(mem_region_requested == NULL){
        printk(KERN_WARNING "[LEO] LED: Failed request_mem_region(res.start,0x10000,...);\n");
    }
    else
        PDEBUG(" [+] request_mem_region\n");

    void* addr_tmp = ioremap(res.start,0x10000);

    // value_read = ioread32(addr_tmp);
    iowrite32(0,addr_tmp + XGPIO_TRI_OFFSET); /* Set output directio */
    iowrite32(LED_value , addr_tmp); /* Set output LED 1 and 4 (binary of decimal 8) */
    PDEBUG(" [+] LED array written : %u \n", LED_value);

    return 0; /* Success */
 }

 static int LED_of_remove(struct platform_device *op)
 {
     release_mem_region(res.start,0x10000);
     PDEBUG(" [+] release_mem_region \n");
     return 0; /* Success */
 }

 static struct platform_driver LED_of_driver = {
      .probe = LED_of_probe,
      .remove = LED_of_remove,
      .driver = {
          .name = "LED_01",
          .owner = THIS_MODULE,
          .of_match_table = LED_of_match,
      },
 };

 /*
  * Open and close (close = release)
  */

 int LED_01_open(struct inode *inode, struct file *filp)
 {
     struct LED_01_dev *dev; /* device information */
     PDEBUG(" performing 'open' operation\n");
     dev = container_of(inode->i_cdev, struct LED_01_dev, cdev);
     filp->private_data = dev; /* for other methods */

     // /* I/O port allocation */
     // if(!request_region(res.start,LED_num_ports,"LED_01")){
     //     printk(KERN_WARNING "[LEO] ERROR requesting resources \n");
     //     return -ENODEV;
     // }

 	   return 0;          /* success */
 }

 int LED_01_release(struct inode *inode, struct file *filp)
 {
     PDEBUG(" performing 'release' operation\n");
     // release_region(res.start,LED_num_ports);
     return 0;
 }

//todo ioclt

ssize_t LED_01_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG(" nothing to do when reading\n");
    return retval;
}

ssize_t LED_01_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG(" nothing to do when reading\n");
    return retval;
}

 /*
  * Create a set of file operations for our LED_01 files.
  * All the functions do nothig
  */

 struct file_operations LED_01_fops = {
     .owner =    THIS_MODULE,
     .read =     LED_01_read,
     .write =    LED_01_write,
     //.unlocked_ioctl = LED_01_ioctl,
     .open =     LED_01_open,
     .release =  LED_01_release,
 };

 /*
  * The cleanup function is used to handle initialization failures as well.
  * Thefore, it must be careful to work correctly even if some of the items
  * have not been initialized
  */

  void LED_01_cleanup_module(void)
 {
     dev_t devno = MKDEV(LED_01_major, LED_01_minor);
     cdev_del(&(LED_01_devices->cdev));
     if((LED_01_devices) != 0){
         kfree(LED_01_devices);
         PDEBUG(" kfree LED_01_devices\n");
     }
     unregister_chrdev_region(devno, LED_01_nr_devs);        /* unregistering device */
     PDEBUG(" cdev deleted, kfree, chdev unregistered\n");
     platform_driver_unregister(&LED_of_driver);             /* unregister PLATFORM driver */
     PDEBUG(" of_unregister_platform_driver\n");
 }

 /*
  * Set up the char_dev structure for this device.
  */

 static void LED_01_setup_cdev(struct LED_01_dev *dev)
 {
 	int err, devno = MKDEV(LED_01_major, LED_01_minor);

 	cdev_init(&dev->cdev, &LED_01_fops);
 	dev->cdev.owner = THIS_MODULE;
 	dev->cdev.ops = &LED_01_fops;
 	err = cdev_add (&dev->cdev, devno, 1);
 	/* Fail gracefully if need be */
 	if (err)
 		printk(KERN_WARNING "[LEO] Error %d adding LED_01 cdev_add", err);

   PDEBUG(" cdev initialized\n");
 }

 /*
  * The init function is used to register the chdeiv allocating dinamically a
  * new major number (if not specified at load/compilation-time)
  */

 static int LED_01_init(void)
 {
 	  int result =0;
 	  dev_t dev = 0;

 	  if (LED_01_major) {
 			  PDEBUG(" static allocation of major number (%d)\n",LED_01_major);
 		    dev = MKDEV(LED_01_major, LED_01_minor);
 		    result = register_chrdev_region(dev, LED_01_nr_devs, "LED_01");
 	  } else {
 			  PDEBUG("LED_01: dinamic allocation of major number\n");
 		    result = alloc_chrdev_region(&dev, LED_01_minor, LED_01_nr_devs, "LED_01");
 		    LED_01_major = MAJOR(dev);
 	  }
 	  if (result < 0) {
 		    printk(KERN_WARNING "[LEO] LED_01: can't get major %d\n", LED_01_major);
 		    return result;
     }

     LED_01_devices = kmalloc(LED_01_nr_devs * sizeof(struct LED_01_dev), GFP_KERNEL);
     if (!LED_01_devices) {
         result = -ENOMEM;
         printk(KERN_WARNING "[LEO] LED_01: ERROR kmalloc dev struct\n");
         goto fail;  /* Make this more graceful */
     }

     memset(LED_01_devices, 0, LED_01_nr_devs * sizeof(struct LED_01_dev));
     /* Initialize the device. */


     sema_init(&(LED_01_devices->sem_LED_01), 1); /* semaphore initialization */
     /* using semaphore because shared variables ( they are global) */
     if (down_interruptible(&(LED_01_devices->sem_LED_01))){
         printk(KERN_WARNING "[LEO] LED_01: Device was busy. Operation aborted\n");
         return -ERESTARTSYS;
     }
     LED_01_setup_cdev(LED_01_devices);
     /* register PLATFORM driver */
     result = platform_driver_register(&LED_of_driver);
     if (result) {
         printk(KERN_ERR "[LEO] ERROR: platform_driver_register() %s\n", DRIVER_NAME);
         goto err_driver;
     }

     up(&(LED_01_devices->sem_LED_01));


     return 0;

     //to fix what happen after an error

     fail:  // kmalloc fail, Only free: chrdev_region
     err_driver: // register platform driver fail, Only free: chrdev_region, kmalloc
     LED_01_cleanup_module();
     return result;
 }

 /*
  * The exit function is simply calls the cleanup
  */

 static void LED_01_exit(void)
 {
 	  PDEBUG(" EXIT: --calling cleanup function--\n");
 	  LED_01_cleanup_module();
 }

 module_init(LED_01_init);
 module_exit(LED_01_exit);

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
 MODULE_DESCRIPTION("example of using LED with ioclt");
 MODULE_VERSION("1.0");
