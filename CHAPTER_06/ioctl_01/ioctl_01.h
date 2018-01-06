#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/*
 *   Debug Macros
 *
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef LEO_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "[LEO] " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif /* __KERNEL__ */
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif /* LEO_DEBUG */

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#define DEVICE_MAX_SIZE (20)

struct ioctl_01_dev {
	char *p_data;                    /* pointer to the memory allocated */
	struct semaphore sem_ioctl_01;   /* semaphore for the struct hello */
	struct cdev cdev;	               /* Char device structure		*/
};

#endif /* _COMMANDS_H_ */
