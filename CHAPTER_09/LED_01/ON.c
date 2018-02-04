 /* ON.c
 *
 * Author  :  Leonardo Suriano<leonardo.suriano@live.it>
 *
 *
 * To compile the file: arm-xilinx-linux-gnueabi-gcc -O -g -DLEO_DEBUG ON.c -o ON
 *
 */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "test_LED_01.h"

// #define LED_TURN_ON    _IO(LED_01_IOC_MAGIC, 0)
// #define LED_TURN_OFF   _IO(LED_01_IOC_MAGIC, 1)
// #define LED_QUERY      _IOR(LED_01_IOC_MAGIC, 2, int)

int main() {

    int fd, result;

    printf("\n-- TEST ioctl_01 device_driver--\n");
    PDEBUG(" LED_TURN_ON:    %u\n",LED_TURN_ON);
    PDEBUG(" LED_TURN_OFF:   %u\n",LED_TURN_OFF);
    PDEBUG(" LED_QUERY:  %u\n",LED_QUERY);
    /* Open operation */
    if ((fd = open("/dev/LED_01", O_RDWR)) < 0 ) {
        perror("1. open failed \n");
        goto fail;
    }
		else{
			  printf("file opend\n");
		}

    /* TURN ON LED */

    int value_ioctl = ioctl(fd,LED_TURN_ON);
    if (value_ioctl < 0){
      printf("value_ioctl : %d\n", value_ioctl);
      printf(" %s\n", strerror(errno));
    }






    /* Close operation */
    if (close(fd)){
		    perror("1. close failed \n");
        goto fail;
		}
		else{
		    printf("file closed\n");
		}
		printf("-- TEST PASSED --\n");
    return 0;
    fail:
    printf("-- TEST FAILED --\n");
    return -1;

}
