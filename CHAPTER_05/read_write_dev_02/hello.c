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
#include <linux/seq_file.h>	     /* for seq_file */
#include <linux/proc_fs.h>
#include "hello.h"


int hello_major = 0;
int hello_minor = 0;
unsigned int hello_nr_devs = 1;
int device_max_size = DEVICE_MAX_SIZE;
unsigned long long read_times = 0;
unsigned long long write_times = 0;

module_param(hello_major, int, S_IRUGO);
module_param(hello_minor, int, S_IRUGO);
//module_param(hello_nr_devs, int, S_IRUGO);
module_param(device_max_size, int, S_IRUGO);

struct hello_dev *hello_devices;	/* allocated in hello_init_module */



/**
 * This function is called at the beginning of a sequence.
 * ie, when:
 *	- the /proc file is read (first time)
 *	- after the function stop (end of sequence)
 *
 */
static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long counter = 0;

	/* beginning a new sequence ? */
  PDEBUGG("[START]: *pos = %llu, counter = %lu \n",*pos,counter);
	if ( *pos < PROC_FILE_PRINTING_TIMES )
	{
		/* yes => return a non null value to begin the sequence */
		return &counter;
	}
	else
	{
		/* no => it's the end of the sequence, return end to stop reading */
		*pos = 0;
		return NULL;
	}
}

/**
 * This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 *
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *)v;
	(*tmp_v)++;
	PDEBUGG("[NEXT] : BEFORE *pos = %llu \n",*pos);
	// (*pos)++;
	// (*pos)++;
	// (*pos)++;
  PDEBUGG("[NEXT] : AFTER  *pos = %llu \n",*pos);
	return NULL;
}

/**
 * This function is called at the end of a sequence
 *
 */
static void my_seq_stop(struct seq_file *s, void *v)
{
	/* nothing to do, we use a static value in start() */
  //loff_t *spos = (loff_t *) v;
  //PDEBUG("[SHOW]: *v = *spos = %Ld \n",*spos);
}

/**
 * This function is called for each "step" of a sequence
 *
 */
static int my_seq_show(struct seq_file *s, void *v)
{
  loff_t *spos = (loff_t *) v;
  if (down_interruptible(&(hello_devices->sem_hello))){
      printk(KERN_WARNING "[LEO] hello: Device was busy. Operation aborted\n");
      return -ERESTARTSYS;
  }
  //seq_printf(s, "Hello Leo in proc_seq_file %Ld\n", *spos);
  seq_printf(s, "Reading /proc/%s\n",PROC_NAME);
  seq_printf(s,"The process is \"%s\" (pid %i)\n",current->comm, current->pid);
  seq_printf(s,"Major number: %d, Minor number: %d\n",hello_major,hello_minor);
  seq_printf(s,"the following table shows how many time the device was read and written:\n\n");
  seq_printf(s,"    ___________________________________\n");
  seq_printf(s,"    |_______read_____|_____write______|\n");
  seq_printf(s,"    |                |                |\n");
  seq_printf(s,"    |    %.8llu    |    %.8llu    |\n",read_times,write_times);
  seq_printf(s,"    |________________|________________|\n\n");
  PDEBUGG("[SHOW] : *v = *spos = %Ld \n",*spos);
  up(&(hello_devices->sem_hello));
	return 0;
}

/**
 * This structure gather "function" to manage the sequence
 *
 */
static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next  = my_seq_next,
	.stop  = my_seq_stop,
	.show  = my_seq_show
};

/**
 * This function is called when the /proc file is open.
 *
 */
static int my_open(struct inode *inode, struct file *file)
{
  PDEBUG("opening proc/ file \n");
	return seq_open(file, &my_seq_ops);
};

/**
 * This structure gather "function" that manage the /proc file
 *
 */
static struct file_operations proc_file_ops = {
	.owner   = THIS_MODULE,
	.open    = my_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};



/*
 * Open and close
 */

int hello_open(struct inode *inode, struct file *filp)
{
    struct hello_dev *dev; /* device information */
    PDEBUG("[LEO] performing 'open' operation\n");
    dev = container_of(inode->i_cdev, struct hello_dev, cdev);
    filp->private_data = dev; /* for other methods */

	  return 0;//seq_open(filp, &my_seq_ops);          /* success */
}

int hello_release(struct inode *inode, struct file *filp)
{
  PDEBUG("[LEO] performing 'release' operation\n");
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
        PDEBUG("[LEO] kfree the string-memory\n");
    }
    if((hello_devices) != 0){
        kfree(hello_devices);
        PDEBUG("[LEO] kfree hello_devices\n");
    }
	  unregister_chrdev_region(devno, hello_nr_devs);        /* unregistering device */
		PDEBUG("[LEO] cdev deleted, kfree, chdev unregistered\n");
    remove_proc_entry(PROC_NAME, NULL);
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
    if (down_interruptible(&(hello_devices->sem_hello))){
        printk(KERN_WARNING "[LEO] hello: Device was busy. Operation aborted\n");
        return -ERESTARTSYS;
    }
    if (copy_to_user(buf, (void*)hello_devices -> p_data, count)) {
        printk(KERN_WARNING "[LEO] hello: can't use copy_to_user. \n");
		    retval = -EPERM;
		    goto out_and_Vsem;
	  }

    out_and_Vsem:
    read_times++;
    up(&(hello_devices->sem_hello));
    out:
    return retval;
}

ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int retval = 0;
    //struct hello_dev *dev = filp->private_data;
    if (count > device_max_size){
        printk(KERN_WARNING "[LEO] hello: trying to write more than possible. Aborting write\n");
        retval = -EFBIG;
        goto out;
    }
    if (down_interruptible(&(hello_devices->sem_hello))){
        printk(KERN_WARNING "[LEO] hello: Device was busy. Operation aborted\n");
        return -ERESTARTSYS;
    }
    if (copy_from_user((void*)hello_devices -> p_data, buf, count)) {
        printk(KERN_WARNING "[LEO] hello: can't use copy_from_user. \n");
        retval = -EPERM;
        goto out_and_Vsem;
    }

    out_and_Vsem:
    write_times++;
    up(&(hello_devices->sem_hello));
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

  PDEBUG("[LEO] cdev initialized\n");
}



/*
 * The init function is used to register the chdeiv allocating dinamically a
 * new major number (if not specified at load/compilation-time)
 */


static int hello_init(void)
{
	  int result =0;
	  dev_t dev = 0;
    struct proc_dir_entry *entry;

	  if (hello_major) {
			  PDEBUG("[LEO] static allocation of major number (%d)\n",hello_major);
		    dev = MKDEV(hello_major, hello_minor);
		    result = register_chrdev_region(dev, hello_nr_devs, "hello");
	  } else {
			  PDEBUG("[LEO] dinamic allocation of major number\n");
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
    sema_init(&(hello_devices->sem_hello), 1); /* semaphore initialization */
    hello_setup_cdev(hello_devices);
    /* creting proc/ file for debugging information */

    PDEBUGG("[init_module] int module function \n");
    entry = proc_create(PROC_NAME, 0, NULL,&proc_file_ops);
    if(entry == NULL)
        return -ENOMEM;

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
	  PDEBUG("[LEO] --calling cleanup function--\n");
	  hello_cleanup_module();
}




module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
MODULE_DESCRIPTION("example of using read and write + debug info");
MODULE_VERSION("1.0");
