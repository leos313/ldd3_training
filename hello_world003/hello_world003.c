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


static int hello_init(void)
{
	int result =0;
	dev_t dev = 0;
	
	printk(KERN_INFO "default values of \n major : %d \n minor : %d \n", hello_world_major, hello_world_minor);
	if (hello_world_major) {
		dev = MKDEV(hello_world_major, hello_world_minor);
		result = register_chrdev_region(dev, hello_nr_devs, "hello_world003");
	} else {
		result = alloc_chrdev_region(&dev, hello_world_minor, hello_nr_devs, "hello_world003");
		hello_world_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "hello_world003: can't get major %d\n", hello_world_major);
		return result;
}
    printk(KERN_INFO "REAL values of \n major : %d \n minor : %d \n", hello_world_major, hello_world_minor);
    return 0;
}
static void hello_exit(void)
{
    printk(KERN_INFO "--NOTHING TO DO--\nGoodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
MODULE_DESCRIPTION("testing pass-parameters");
MODULE_VERSION("1.0");
