/*
 * 
 * Author  :   Leonardo Suriano <leonardo.suriano@live.it>
 * Date    :   Dicember 2017
 * 
 * this device driver was written to try to pass parameter during the load process of a new module
 * 
 * USE:
 * to insert the module, use the command 
 * $ insmod hello_world002.ko N_time=10 who="Leo"
 * 
 * 
 * 
 * */



#include <linux/init.h>          /* needed for module_init and exit */
#include <linux/module.h>
#include <linux/moduleparam.h>   /* needed for module_param */
#include <linux/kernel.h>        /* needed for printk */
#include <linux/sched.h>         /* needed for current-> */

static char *who = "world";
static int N_time = 1;

module_param(N_time, int, S_IRUGO);
module_param(who, charp, S_IRUGO);

static int hello_init(void)
{
	int i=0;
	printk(KERN_INFO "The process is \"%s\" (pid %i)\n", current->comm, current->pid);
	for (i=0;i<N_time;i++){
        printk(KERN_ALERT "Hello %s\n",who);
	}
    return 0;
}
static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye %s\n",who);
    printk(KERN_INFO "The process is \"%s\" (pid %i)\n", current->comm, current->pid);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
MODULE_DESCRIPTION("testing pass-parameters");
MODULE_VERSION("1.0");
