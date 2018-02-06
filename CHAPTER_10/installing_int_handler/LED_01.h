#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <linux/ioctl.h>

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

#define DRIVER_NAME "LED_01"

/* Hardware information */

#define XGPIO_DATA_OFFSET	  0x0    /* < Data register for 1st channel      */
#define XGPIO_TRI_OFFSET	  0x4    /* < I/O direction reg for 1st channel*/
#define XGPIO_DATA2_OFFSET	0x8    /* < Data register for 2nd channel*/
#define XGPIO_TRI2_OFFSET	  0xC    /* < I/O direction reg for 2nd channel*/
#define XGPIO_GIE_OFFSET	  0x11C  /* < Glogal interrupt enable register*/
#define XGPIO_ISR_OFFSET	  0x120  /* < Interrupt status register*/
#define XGPIO_IER_OFFSET	  0x128  /* < Interrupt enable register*/

struct LED_01_dev {
  u32 LED_value;
  struct resource res;           /* to store platform info */
  struct resource* temp_res;           /* to store platform info */
  struct resource* mem_region_requested;
  void* addr_tmp;
	struct semaphore sem_LED_01;   /* semaphore for the struct hello */
	struct cdev cdev;	             /* Char device structure		*/
};

/*
 * LED definitions
 */

/* Use 'k' as magic number */
#define LED_01_IOC_MAGIC  'Y'
/* Please use a different 8-bit number in your code */

#define LED_TURN_ON    _IO(LED_01_IOC_MAGIC, 0)
#define LED_TURN_OFF   _IO(LED_01_IOC_MAGIC, 1)
#define LED_QUERY      _IOR(LED_01_IOC_MAGIC, 2, int)

#define LED_01_IOC_MAXNR (2)


#define LED_DIRECTION_OUTPUT (0)
#define LED_DIRECTION_INPUT (1)

#define COMMAND_MAX_LENGHT (1)

#endif /* _COMMANDS_H_ */
