/* hellotest.c
*
* Author  :  Leonardo Suriano<leonardo.suriano@live.it>
*
* A simple example of a C program to test some of the
* operations of the "/dev/hello" device (a.k.a "hello0"),
*
* To compile the file: gcc test_complete_read.c -o test_complete_read.elf
*
*/
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main() {

   int fd, result;

   printf("\n-- TEST complete device_driver--\n");
   /* Open operation */
   if ((fd = open("/dev/complete", O_RDWR)) < 0 ) {
       perror("1. open failed \n");
       goto fail;
   }
   else{
       printf("file opend\n");
   }


   /* Read operation */

   char b[1000];
   //reading a


   //reading b
   result = read(fd, (void*)b, 7);
   if ( result != 0 ){
      printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
   }
   else{
      printf("string read %s \n",b);
   }

   /* Close operation */
   if (close(fd)){
       perror("1. close failed \n");
   }
   else{
       printf("file closed\n");
   }
   printf("-- TEST PASSED --\n");
   return 0;
   fail:
   return -1;

}
