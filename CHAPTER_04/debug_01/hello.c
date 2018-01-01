/*
 *
 * Author  :   Leonardo Suriano <leonardo.suriano@live.it>
 * Date    :   Dicember 2017
 *
 *
 *
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

#include "hello.h"


int hello_major = 0;
int hello_minor = 0;
unsigned int hello_nr_devs = 1;
int device_max_size = DEVICE_MAX_SIZE;

module_param(hello_major, int, S_IRUGO);
module_param(hello_minor, int, S_IRUGO);
//module_param(hello_nr_devs, int, S_IRUGO);
module_param(device_max_size, int, S_IRUGO);

struct hello_dev *hello_devices;	/* allocated in hello_init_module */


/*
 * Open and close
 */

int hello_open(struct inode *inode, struct file *filp)
{
    struct hello_dev *dev; /* device information */
    PDEBUG("performing 'open' operation\n");
    dev = container_of(inode->i_cdev, struct hello_dev, cdev);
    filp->private_data = dev; /* for other methods */

	  return 0;          /* success */
}

int hello_release(struct inode *inode, struct file *filp)
{
    PDEBUG("performing 'release' operation\n");
    return 0;
}

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */

 void hello_cleanup_module(void)
{
	  dev_t devno = MKDEV(hello_major, hello_minor);
    cdev_del(&(hello_devices->cdev));
    /* freeing the memory */
    if((hello_devices -> p_data) != 0){
        kfree(hello_devices -> p_data);
        PDEBUG("kfree the string-memory\n");
    }
    if((hello_devices) != 0){
        kfree(hello_devices);
        PDEBUG("kfree hello_devices\n");
    }
	  unregister_chrdev_region(devno, hello_nr_devs);        /* unregistering device */
    PDEBUG("cdev deleted, kfree, chdev unregistered\n");
}

/*
 * Data management: read and write
 */

ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    //struct hello_dev *dev = filp->private_data;
    ssize_t retval = 0;
    if (count > device_max_size){
        printk(KERN_WARNING "[LEO] hello: trying to read more than possible. Aborting read\n");
        retval = -EFBIG;
        goto out;
    }
    if (copy_to_user(buf, (void*)hello_devices -> p_data, count)) {
        printk(KERN_WARNING "[LEO] hello: can't use copy_to_user. \n");
		    retval = -EPERM;
		    goto out;
	  }
    PDEBUG("performed READ Operation successfully\n");
    return retval;

    out:
    return retval;
}

ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int retval = 0;
    struct hello_dev *dev = filp->private_data;
    if (count > device_max_size){
        printk(KERN_WARNING "[LEO] hello: trying to write more than possible. Aborting write\n");
        retval = -EFBIG;
        goto out;
    }
    if (copy_from_user((void*)hello_devices -> p_data, buf, count)) {
        printk(KERN_WARNING "[LEO] hello: can't use copy_from_user. \n");
        retval = -EPERM;
        goto out;
    }
    PDEBUG("performed WRITE Operation successfully\n");
    return retval;

    out:
    return retval;
}

/*
 * Create a set of file operations for our hello files.
 * All the functions do nothig
 */

struct file_operations hello_fops = {
    .owner =    THIS_MODULE,
    .read =     hello_read,
    .write =    hello_write,
    .open =     hello_open,
    .release =  hello_release,
};


/*
 * Set up the char_dev structure for this device.
 */
static void hello_setup_cdev(struct hello_dev *dev)
{
	int err, devno = MKDEV(hello_major, hello_minor);

	cdev_init(&dev->cdev, &hello_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &hello_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_WARNING "[LEO] Error %d adding hello cdev_add", err);

  PDEBUG("cdev initialized\n");
}



/*
 * The init function is used to register the chdeiv allocating dinamically a
 * new major number (if not specified at load/compilation-time)
 */


static int hello_init(void)
{
	  int result =0;
	  dev_t dev = 0;

	  if (hello_major) {
        PDEBUG("static allocation of major number (%d)\n",hello_major);
			  printk(KERN_INFO "[LEO] static allocation of major number (%d)\n",hello_major);
		    dev = MKDEV(hello_major, hello_minor);
		    result = register_chrdev_region(dev, hello_nr_devs, "hello");
	  } else {
        PDEBUG("dinamic allocation of major number\n");
		    result = alloc_chrdev_region(&dev, hello_minor, hello_nr_devs, "hello");
		    hello_major = MAJOR(dev);
	  }
	  if (result < 0) {
		    printk(KERN_WARNING "[LEO] hello: can't get major %d\n", hello_major);
		    return result;
    }

    hello_devices = kmalloc(hello_nr_devs * sizeof(struct hello_dev), GFP_KERNEL);
    if (!hello_devices) {
        result = -ENOMEM;
        printk(KERN_WARNING "[LEO] ERROR kmalloc dev struct\n");
        goto fail;  /* Make this more graceful */
    }

    memset(hello_devices, 0, hello_nr_devs * sizeof(struct hello_dev));
    /* Initialize the device. */

    hello_devices -> p_data = (char*)kmalloc(device_max_size * sizeof(char), GFP_KERNEL);
    if (!hello_devices -> p_data) {
        result = -ENOMEM;
        printk(KERN_WARNING "[LEO] ERROR kmalloc p_data\n");
        goto fail;  /* Make this more graceful */
    }
    hello_setup_cdev(hello_devices);

    return 0;

    fail:
    hello_cleanup_module();
    return result;
}

/*
 * The exit function is simply calls the cleanup
 */

static void hello_exit(void)
{
    PDEBUG("--calling cleanup function--\n");
	  hello_cleanup_module();
}






module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
MODULE_DESCRIPTION("example of using read and write");
MODULE_VERSION("1.0");
