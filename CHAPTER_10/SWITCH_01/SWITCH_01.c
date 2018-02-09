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
 #include <linux/ioport.h>         /* I/O port allocation request_resource(...), resource_size(..) */
 #include <linux/interrupt.h>      /* Interrupt functions */
 #include <linux/of_irq.h>  /* irq_of_parse_and_map */

 #include "SWITCH_01.h"

 int SWITCH_01_major = 0;
 int SWITCH_01_minor = 0;
 unsigned int SWITCH_01_nr_devs = 1;
 unsigned long long read_times = 0;
 unsigned long long write_times = 0;

 struct SWITCH_01_dev *SWITCH_01_devices;	/* allocated in SWITCH_01_init_module */

 /* PLATFORM DRIVER */

 /* Match table for of_platform binding */
 static struct of_device_id SWITCH_of_match[] = {
  {
    //.compatible = "xlnx,xps-gpio-1.00.a,generic-uio,SWITCH",
    .compatible = "xlnx,xps-gpio-1.00.a,generic-uio,BTandSW",
  },
  {}
 };

 MODULE_DEVICE_TABLE(of, SWITCH_of_match);

 unsigned long *base_addr; /* Virtual Base Address */
 /* platform device structures */

 irqreturn_t SWITCH_01_interrupt(int irq, void *dev_id)
{
    printk(KERN_ALERT "NOTHING TO DO WITH THE INTERRUPT\n");
    return IRQ_HANDLED;
}


 static int SWITCH_of_probe(struct platform_device *pdev)
 {
     int ret=0;

 	   SWITCH_01_devices->temp_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
 	   if (!(SWITCH_01_devices->temp_res)) {
 	   	dev_err(&pdev->dev, "TRY could not get IO memory\n");
 	   	return -ENXIO;
 	   }

     PDEBUG("resource : regs.start=%#x,regs.end=%#x\n",SWITCH_01_devices->temp_res->start,SWITCH_01_devices->temp_res->end);

 	   //SWITCH_01_devices->irq_line = platform_get_irq(pdev, 0);
     SWITCH_01_devices->irq_line = irq_of_parse_and_map(pdev->dev.of_node, 0);
 	   if (SWITCH_01_devices->irq_line < 0) {
 	       dev_err(&pdev->dev, "could not get IRQ\n");
         printk(KERN_ALERT "could not get IRQ\n");
 	       return SWITCH_01_devices->irq_line;
 	   }
     PDEBUG("resource : irq=%#x\n",SWITCH_01_devices->irq_line);
     ret = request_irq(29, SWITCH_01_interrupt, IRQF_SHARED	, DRIVER_NAME, NULL);
     if (ret) {
        printk(KERN_ALERT "SWITCH_01: can't get assigned irq %i, ret= %d\n", SWITCH_01_devices->irq_line, ret);
        SWITCH_01_devices->irq_line = -1;
    }

     //printk("TRY resource : num_clock=%d\n", pdev->dev.platform_data.num_clock);


     SWITCH_01_devices->mem_region_requested = request_mem_region((SWITCH_01_devices->temp_res->start),resource_size(SWITCH_01_devices->temp_res),"SWITCH_01");
     if(SWITCH_01_devices->mem_region_requested == NULL){
         printk(KERN_WARNING "[LEO] SWITCH: FaiSWITCH request_mem_region(res.start,resource_size(&(SWITCH_01_devices->res)),...);\n");
     }
     else
         PDEBUG(" [+] request_mem_region\n");

     return 0; /* Success */
 }

 static int SWITCH_of_remove(struct platform_device *op)
 {
     release_mem_region((SWITCH_01_devices->temp_res->start),resource_size(SWITCH_01_devices->temp_res));
     PDEBUG(" [+] release_mem_region \n");
     free_irq(SWITCH_01_devices->irq_line, NULL);
     return 0; /* Success */
 }

 static struct platform_driver SWITCH_of_driver = {
      .probe = SWITCH_of_probe,
      .remove = SWITCH_of_remove,
      .driver = {
          .name = "SWITCH_01",
          .owner = THIS_MODULE,
          .of_match_table = SWITCH_of_match,
      },
 };

 /*
  * Open and close (close = release)
  */

 int SWITCH_01_open(struct inode *inode, struct file *filp)
 {
     struct SWITCH_01_dev *dev; /* device information */
     PDEBUG(" performing 'open' operation\n");
     dev = container_of(inode->i_cdev, struct SWITCH_01_dev, cdev);
     filp->private_data = dev; /* for other methods */

     // /* I/O port allocation */
     // if(!request_region(res.start,SWITCH_num_ports,"SWITCH_01")){
     //     printk(KERN_WARNING "[LEO] ERROR requesting resources \n");
     //     return -ENODEV;
     // }

 	   return 0;          /* success */
 }

 int SWITCH_01_release(struct inode *inode, struct file *filp)
 {
     PDEBUG(" performing 'release' operation\n");
     // release_region(res.start,SWITCH_num_ports);
     return 0;
 }



 /*
  * The ioctl() implementation
  */

 long SWITCH_01_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
 {
 	int retval = 0;
  int err = 0;
  int value_read;
  /*
   * extract the type and number bitfields, and don't decode
   * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
   */
  if (_IOC_TYPE(cmd) != SWITCH_01_IOC_MAGIC) {
    PDEBUG(" _IOC_TYPE(cmd) :      %u\n",_IOC_TYPE(cmd));
    PDEBUG(" SWITCH_01_IOC_MAGIC:   %u\n",SWITCH_01_IOC_MAGIC);
    printk(KERN_WARNING "[LEO] SWITCH_01: _IOC_TYPE(cmd) != SWITCH_01_IOC_MAGIC => false. Aborting ioctl\n");
    return -ENOTTY;
  }
  if (_IOC_NR(cmd) > SWITCH_01_IOC_MAXNR) {
    printk(KERN_WARNING "[LEO] SWITCH_01: _IOC_NR(cmd) > SWITCH_01_IOC_MAXNR => false. Aborting ioctl\n");
    return -ENOTTY;
  }
  /*
   * the direction is a bitmask, and VERIFY_WRITE catches R/W
   * transfers. `Type' is user-oriented, while
   * access_ok is kernel-oriented, so the concept of "read" and
   * "write" is reversed
   */
  if (_IOC_DIR(cmd) & _IOC_READ)
    err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
  else if (_IOC_DIR(cmd) & _IOC_WRITE)
    err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
  if (err) return -EFAULT;

  switch(cmd) {
    case SWITCH_TURN_ON:
    PDEBUG(" SWITCH_TURN_ON\n");
    /* using semaphore because of global (shared) variable */
    if (down_interruptible(&(SWITCH_01_devices->sem_SWITCH_01))){
        printk(KERN_WARNING "[LEO] ioctl_01: Device was busy. Operation aborted\n");
        return -ERESTARTSYS;
    }
    // nothing to do
    PDEBUG(" [+] SWITCH is now ON : %u \n", SWITCH_01_devices->SWITCH_value);

    up(&(SWITCH_01_devices->sem_SWITCH_01));
    break;
    case SWITCH_TURN_OFF:
    PDEBUG(" SWITCH_TURN_OFF\n");
    /* using semaphore because of global (shared) variable */
    if (down_interruptible(&(SWITCH_01_devices->sem_SWITCH_01))){
        printk(KERN_WARNING "[LEO] ioctl_01: Device was busy. Operation aborted\n");
        return -ERESTARTSYS;
    }

    PDEBUG(" [+] SWITCH is now OFF : %u \n", SWITCH_01_devices->SWITCH_value);
    // nothing to do
    up(&(SWITCH_01_devices->sem_SWITCH_01));
    break;
    case SWITCH_QUERY:
    if (down_interruptible(&(SWITCH_01_devices->sem_SWITCH_01))){
        printk(KERN_WARNING "[LEO] ioctl_01: Device was busy. Operation aborted\n");
        return -ERESTARTSYS;
    }
    SWITCH_01_devices->addr_tmp = ioremap((SWITCH_01_devices->temp_res->start),resource_size(SWITCH_01_devices->temp_res));
    iowrite32(SWITCH_DIRECTION_INPUT,SWITCH_01_devices->addr_tmp + XGPIO_TRI_OFFSET); /* Set output directio */
    up(&(SWITCH_01_devices->sem_SWITCH_01));
    value_read = ioread32(SWITCH_01_devices->addr_tmp);
    iounmap(SWITCH_01_devices->addr_tmp);
    PDEBUG(" SWITCH_QUERY value_read: %d \n",value_read);
    return value_read;
    break;
    default:  /* redundant, as cmd was checked against MAXNR */
    return -ENOTTY;
 	}

 	return retval;

 }

ssize_t SWITCH_01_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG(" nothing to do when reading\n");

    return retval;
}

ssize_t SWITCH_01_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  ssize_t retval = 0;
  PDEBUG(" nothing to do when writing\n");

  return retval;
}

 /*
  * Create a set of file operations for our SWITCH_01 files.
  * All the functions do nothig
  */

 struct file_operations SWITCH_01_fops = {
     .owner =    THIS_MODULE,
     .read =     SWITCH_01_read,
     .write =    SWITCH_01_write,
     .unlocked_ioctl = SWITCH_01_ioctl,
     .open =     SWITCH_01_open,
     .release =  SWITCH_01_release,
 };

 /*
  * The cleanup function is used to handle initialization failures as well.
  * Thefore, it must be careful to work correctly even if some of the items
  * have not been initialized
  */

  void SWITCH_01_cleanup_module(void)
 {
     dev_t devno = MKDEV(SWITCH_01_major, SWITCH_01_minor);
     cdev_del(&(SWITCH_01_devices->cdev));
     if((SWITCH_01_devices) != 0){
         kfree(SWITCH_01_devices);
         PDEBUG(" kfree SWITCH_01_devices\n");
     }
     unregister_chrdev_region(devno, SWITCH_01_nr_devs);        /* unregistering device */
     PDEBUG(" cdev deleted, kfree, chdev unregistered\n");
     platform_driver_unregister(&SWITCH_of_driver);             /* unregister PLATFORM driver */
     PDEBUG(" of_unregister_platform_driver\n");
 }

 /*
  * Set up the char_dev structure for this device.
  */

 static void SWITCH_01_setup_cdev(struct SWITCH_01_dev *dev)
 {
 	int err, devno = MKDEV(SWITCH_01_major, SWITCH_01_minor);

 	cdev_init(&dev->cdev, &SWITCH_01_fops);
 	dev->cdev.owner = THIS_MODULE;
 	dev->cdev.ops = &SWITCH_01_fops;
 	err = cdev_add (&dev->cdev, devno, 1);
 	/* Fail gracefully if need be */
 	if (err)
 		printk(KERN_WARNING "[LEO] Error %d adding SWITCH_01 cdev_add", err);

   PDEBUG(" cdev initialized\n");
 }

 /*
  * The init function is used to register the chdeiv allocating dinamically a
  * new major number (if not specified at load/compilation-time)
  */

 static int SWITCH_01_init(void)
 {
 	  int result =0;
 	  dev_t dev = 0;

 	  if (SWITCH_01_major) {
 			  PDEBUG(" static allocation of major number (%d)\n",SWITCH_01_major);
 		    dev = MKDEV(SWITCH_01_major, SWITCH_01_minor);
 		    result = register_chrdev_region(dev, SWITCH_01_nr_devs, "SWITCH_01");
 	  } else {
 			  PDEBUG("SWITCH_01: dinamic allocation of major number\n");
 		    result = alloc_chrdev_region(&dev, SWITCH_01_minor, SWITCH_01_nr_devs, "SWITCH_01");
 		    SWITCH_01_major = MAJOR(dev);
 	  }
 	  if (result < 0) {
 		    printk(KERN_WARNING "[LEO] SWITCH_01: can't get major %d\n", SWITCH_01_major);
 		    return result;
     }

     SWITCH_01_devices = kmalloc(SWITCH_01_nr_devs * sizeof(struct SWITCH_01_dev), GFP_KERNEL);
     if (!SWITCH_01_devices) {
         result = -ENOMEM;
         printk(KERN_WARNING "[LEO] SWITCH_01: ERROR kmalloc dev struct\n");
         goto fail;  /* Make this more graceful */
     }

     memset(SWITCH_01_devices, 0, SWITCH_01_nr_devs * sizeof(struct SWITCH_01_dev));
     /* Initialize the device. */


     sema_init(&(SWITCH_01_devices->sem_SWITCH_01), 1); /* semaphore initialization */
     /* using semaphore because shared variables ( they are global) */
     if (down_interruptible(&(SWITCH_01_devices->sem_SWITCH_01))){
         printk(KERN_WARNING "[LEO] SWITCH_01: Device was busy. Operation aborted\n");
         return -ERESTARTSYS;
     }
     SWITCH_01_setup_cdev(SWITCH_01_devices);
     /* register PLATFORM driver */
     result = platform_driver_register(&SWITCH_of_driver);
     if (result) {
         printk(KERN_ERR "[LEO] ERROR: platform_driver_register() %s\n", DRIVER_NAME);
         goto err_driver;
     }

     up(&(SWITCH_01_devices->sem_SWITCH_01));


     return 0;

     //to fix what happen after an error

     fail:  // kmalloc fail, Only free: chrdev_region
     err_driver: // register platform driver fail, Only free: chrdev_region, kmalloc
     SWITCH_01_cleanup_module();
     return result;
 }

 /*
  * The exit function is simply calls the cleanup
  */

 static void SWITCH_01_exit(void)
 {
 	  PDEBUG(" EXIT: --calling cleanup function--\n");
 	  SWITCH_01_cleanup_module();
 }

 module_init(SWITCH_01_init);
 module_exit(SWITCH_01_exit);

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
 MODULE_DESCRIPTION("example of using SWITCH with ioclt");
 MODULE_VERSION("1.0");
