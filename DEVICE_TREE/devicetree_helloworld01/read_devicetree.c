/*
 *
 * Author  :   Leonardo Suriano <leonardo.suriano@live.it>
 * Date    :   Dicember 2017
 *
 *
 * to be loaded with insmod "read_devicetree.ko"
 *
 * */



#include <linux/init.h>          /* needed for module_init and exit */
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>   /* needed for module_param */
#include <linux/kernel.h>        /* needed for printk */
#include <linux/sched.h>         /* needed for current-> */
#include <linux/types.h>         /* needed for dev_t type */
#include <linux/kdev_t.h>        /* needed for macros MAJOR, MINOR, MKDEV... */
#include <linux/fs.h>            /* needed for register_chrdev_region */
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <asm/io.h>


int hello_world_major = 0;
int hello_world_minor = 0;
unsigned int hello_nr_devs = 1;
#define DRIVER_NAME "LEO_EXAMPLE"

module_param(hello_world_major, int, S_IRUGO);
module_param(hello_world_minor, int, S_IRUGO);
module_param(hello_nr_devs, int, S_IRUGO);



/* PLATFORM DRIVER */

/* Match table for of_platform binding */
static struct of_device_id LEO_of_match[] = {
 { .compatible = "xlnx,xps-gpio-1.00.a,generic-uio", },
 {}
};

MODULE_DEVICE_TABLE(of, LEO_of_match);
//MODULE_ALIAS("LEO");

static struct resource res;

static int LEO_of_probe(struct platform_device *op)
{
    int ret;

    ret = of_address_to_resource(op->dev.of_node, 0, &res);
    if (ret) {
        printk(KERN_WARNING "LEO: Failed to obtain device tree resource\n");
        return ret;
    }

    printk(KERN_WARNING "LEO: Physical address to resource is %x\n", (unsigned int) res.start);

    return 0; /* Success */
}

static int LEO_of_remove(struct platform_device *op)
{
    return 0; /* Success */
}

static struct platform_driver LEO_of_driver = {
     .probe = LEO_of_probe,
     .remove = LEO_of_remove,
     .driver = {
         .name = "LEO",
         .owner = THIS_MODULE,
         .of_match_table = LEO_of_match,
     },
};



/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */

 void hello_world_cleanup_module(void)
{
	  dev_t devno = MKDEV(hello_world_major, hello_world_minor);
	  unregister_chrdev_region(devno, hello_nr_devs);
		printk(KERN_INFO "[LEO] chdev unregistered\n");
    platform_driver_unregister(&LEO_of_driver);
    printk(KERN_INFO "[LEO] of_unregister_platform_driver\n");
}

/*
 * The init function is used to register the chdeiv allocating dinamically a
 * new major number (if not specified at load/compilation-time)
 */


static int hello_init(void)
{
	  int result =0;
	  dev_t dev = 0;

	  printk(KERN_INFO "[LEO] default values of \n[LEO] --major : %d \n[LEO] --minor : %d \n", hello_world_major, hello_world_minor);
	  if (hello_world_major) {
			  printk(KERN_INFO "[LEO] static allocation of major number (%d)\n",hello_world_major);
		    dev = MKDEV(hello_world_major, hello_world_minor);
		    result = register_chrdev_region(dev, hello_nr_devs, "hello_world003");
	  } else {
			  printk(KERN_INFO "[LEO] dinamic allocation of major number\n");
		    result = alloc_chrdev_region(&dev, hello_world_minor, hello_nr_devs, "hello_world003");
		    hello_world_major = MAJOR(dev);
	  }
	  if (result < 0) {
		    printk(KERN_WARNING "[LEO] hello_world003: can't get major %d\n", hello_world_major);
		    return result;
    }
    printk(KERN_INFO "[LEO] values of \n[LEO] --major : %d \n[LEO] --minor : %d \n", hello_world_major, hello_world_minor);
    // Register platform driver
    printk(KERN_INFO "[LEO] platform_driver_register() %s\n", DRIVER_NAME);


    result = platform_driver_register(&LEO_of_driver);
    if (result) {
        printk(KERN_ERR "[LEO] ERROR: platform_driver_register() %s\n", DRIVER_NAME);
        goto err_driver;
    }
    printk(KERN_INFO "[LEO] platform_driver_register() DONE %s\n", DRIVER_NAME);

    printk(KERN_INFO "[LEO] init() DONE %s\n", DRIVER_NAME);

    return 0;

    err_driver:
    hello_world_cleanup_module();
    return result;
}

/*
 * The exit function is simply calls the cleanup
 */

static void hello_exit(void)
{
	  printk(KERN_INFO "[LEO] --calling cleanup function--\n");
	  hello_world_cleanup_module();
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
MODULE_DESCRIPTION("reading parameter from device tree");
MODULE_VERSION("1.0");
