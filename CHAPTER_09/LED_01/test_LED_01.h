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


/*
 * LED Ioctl definitions
 */

 /* Use 'k' as magic number */
 #define LED_01_IOC_MAGIC  'Y'
 /* Please use a different 8-bit number in your code */

 #define LED_TURN_ON    _IO(LED_01_IOC_MAGIC, 0)
 #define LED_TURN_OFF   _IO(LED_01_IOC_MAGIC, 1)
 #define LED_QUERY      _IOR(LED_01_IOC_MAGIC, 2, int)

 #define LED_01_IOC_MAXNR (2)


#endif /* _COMMANDS_H_ */
