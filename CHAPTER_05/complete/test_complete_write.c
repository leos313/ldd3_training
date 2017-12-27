/* hellotest.c
*
* Author  :  Leonardo Suriano<leonardo.suriano@live.it>
*
* A simple example of a C program to test some of the
* operations of the "/dev/hello" device (a.k.a "hello0"),
*
* To compile the file: gcc test_complete_write.c -o test_complete_write.elf
*
*/
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main() {

   int fd, result;

   printf("\n-- TEST 'complete' module--\n");
   /* Open operation */
   if ((fd = open("/dev/complete", O_RDWR)) < 0 ) {
       perror("1. open failed \n");
       goto fail;
   }
   else{
       printf("file opend\n");
   }

   /* Write operation */

   char w_b[12];
   strcpy(w_b,"1111111");
   result = write(fd, (void*) w_b, 12);
   if ( result != 0 ){
       printf("ERROR: %d -> %s\n", result, strerror(errno));
   }
   else{
       printf("write operation executed succesfully\n");
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
