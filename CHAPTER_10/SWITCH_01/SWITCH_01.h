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

#define DRIVER_NAME "SWITCH_01"

/* Hardware information */

/** @name Registers
 *
 * Register offsets for this device.
 * @{
 */
#define XGPIO_DATA_OFFSET	  0x0    /* < Data register for 1st channel      */
#define XGPIO_TRI_OFFSET	  0x4    /* < I/O direction reg for 1st channel*/
#define XGPIO_DATA2_OFFSET	0x8    /* < Data register for 2nd channel*/
#define XGPIO_TRI2_OFFSET	  0xC    /* < I/O direction reg for 2nd channel*/
#define XGPIO_GIE_OFFSET	  0x11C  /* < Glogal interrupt enable register*/
#define XGPIO_ISR_OFFSET	  0x120  /* < Interrupt status register*/
#define XGPIO_IER_OFFSET	  0x128  /* < Interrupt enable register*/

/*@}*/

/* The following constant describes the offset of each channels data and
 * tristate register from the base address.
 */
#define XGPIO_CHAN_OFFSET  8

/** @name Interrupt Status and Enable Register bitmaps and masks
 *
 * Bit definitions for the interrupt status register and interrupt enable
 * registers.
 * @{
 */
#define XGPIO_IR_MASK		0x3 /**< Mask of all bits */
#define XGPIO_IR_CH1_MASK	0x1 /**< Mask for the 1st channel */
#define XGPIO_IR_CH2_MASK	0x2 /**< Mask for the 2nd channel */
/*@}*/


/** @name Global Interrupt Enable Register bitmaps and masks
 *
 * Bit definitions for the Global Interrupt  Enable register
 * @{
 */
#define XGPIO_GIE_GINTR_ENABLE_MASK	0x80000000
/*@}*/




struct SWITCH_01_dev {
  int irq_line;     /* interrupt line used by the SW */
  u32 N_interrupts;
  u32 SWITCH_value;
  struct resource* temp_res;           /* to store platform info */
  struct resource* mem_region_requested;
  void* addr_tmp;
	struct semaphore sem_SWITCH_01;   /* semaphore for the struct hello */
	struct cdev cdev;	             /* Char device structure		*/
};

/*
 * SWITCH definitions
 */

/* Use 'k' as magic number */
#define SWITCH_01_IOC_MAGIC  'Y'
/* Please use a different 8-bit number in your code */

#define SWITCH_TURN_ON    _IO(SWITCH_01_IOC_MAGIC, 0)
#define SWITCH_TURN_OFF   _IO(SWITCH_01_IOC_MAGIC, 1)
#define SWITCH_QUERY      _IOR(SWITCH_01_IOC_MAGIC, 2, int)

#define SWITCH_01_IOC_MAXNR (2)


#define SWITCH_DIRECTION_OUTPUT (0)
#define SWITCH_DIRECTION_INPUT (1)

#define COMMAND_MAX_LENGHT (1)

#endif /* _COMMANDS_H_ */
