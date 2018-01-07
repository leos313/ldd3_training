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
	char *p_data_01;                 /* first memory buffer */
	char *p_data_02;                 /* second memory buffer */
	struct semaphore sem_ioctl_01;   /* semaphore for the struct hello */
	struct cdev cdev;	               /* Char device structure		*/
};

/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define IOCTL_01_IOC_MAGIC  'T'
/* Please use a different 8-bit number in your code */

#define DEVICE_IOCRESET    _IO(IOCTL_01_IOC_MAGIC, 0)
#define SET_FIRST_BUFFER   _IO(IOCTL_01_IOC_MAGIC, 1)
#define SET_SECOND_BUFFER  _IO(IOCTL_01_IOC_MAGIC, 2)
#define WHICH_BUFFER       _IOR(IOCTL_01_IOC_MAGIC, 3, int)

#define IOCTL_01_IOC_MAXNR (3)


#endif /* _COMMANDS_H_ */
