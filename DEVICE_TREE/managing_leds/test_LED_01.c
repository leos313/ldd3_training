/* test_LED_01.c
*
* Author  :  Leonardo Suriano<leonardo.suriano@live.it>
*
* A simple example of a C program to test some of the driver for the LEDs on Pynq board
*
* To compile the file: gcc -O -g -DLEO_DEBUG test_LED_01.c -o test_LED_01.elf
*
*/

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>


#define DEVICE_NAME "/dev/LED_01"

int main() {

   int fd, result;

   /* Open operation */
   if ((fd = open(DEVICE_NAME, O_RDWR)) < 0 ) {
       printf("\n");
       perror("1. open failed \n");
       printf("you were looking for %s \n",DEVICE_NAME);
       goto fail;
   }
   else{
       printf("file opend\n");
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
