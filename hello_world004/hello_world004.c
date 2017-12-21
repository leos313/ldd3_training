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
#include <linux/fs.h>            /* needed for register_chrdev_region */


int hello_world_major = 0;
int hello_world_minor = 0;
unsigned int hello_nr_devs = 1;

module_param(hello_world_major, int, S_IRUGO);
module_param(hello_world_minor, int, S_IRUGO);
module_param(hello_nr_devs, int, S_IRUGO);




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
		    result = register_chrdev_region(dev, hello_nr_devs, "hello_world004");
	  } else {
			  printk(KERN_INFO "[LEO] dinamic allocation of major number\n");
		    result = alloc_chrdev_region(&dev, hello_world_minor, hello_nr_devs, "hello_world004");
		    hello_world_major = MAJOR(dev);
	  }
	  if (result < 0) {
		    printk(KERN_WARNING "[LEO] hello_world004: can't get major %d\n", hello_world_major);
		    return result;
    }
    printk(KERN_INFO "[LEO] values of \n[LEO] --major : %d \n[LEO] --minor : %d \n", hello_world_major, hello_world_minor);
    return 0;
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
MODULE_DESCRIPTION("testing pass-parameters");
MODULE_VERSION("1.0");
