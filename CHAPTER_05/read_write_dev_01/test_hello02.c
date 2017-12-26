/* hellotest.c
*
* Author  :  Leonardo Suriano<leonardo.suriano@live.it>
*
* A simple example of a C program to test some of the
* operations of the "/dev/hello" device (a.k.a "hello0"),
*
* To compile the file: gcc test_hello02.c -o test_hello02.elf
*
*/
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main() {

   int fd, result;

   printf("\n-- TEST hello device_driver--\n");
   /* Open operation */
   if ((fd = open("/dev/hello", O_RDWR)) < 0 ) {
       perror("1. open failed \n");
       goto fail;
   }
   else{
       printf("file opend\n");
   }

   /* Write operation */

   char w_b[12];
   strcpy(w_b,"test_02");
   result = write(fd, (void*) w_b, 12);
   if ( result != 0 ){
       printf("Oh dear, something went wrong with write()! %s\n", strerror(errno));
   }
   else{
       printf("write operation executed succesfully\n");
   }

   /* Read operation */

   char a[1000];
   char b[1000];
   //reading a
   while(1){
       result = write(fd, (void*) w_b, 12);
       if ( result != 0 ){
           printf("Oh dear, something went wrong with write()! %s\n", strerror(errno));
           goto fail;
       }
       //reading b
       result = read(fd, (void*)b, 7);
       if ( result != 0 ){
           printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
           goto fail;
       }
       else{
           printf("string read %s \n",b);
       }
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
   if (close(fd)){
       perror("1. close failed \n");
   }
   printf("-- TEST FAILED --\n");
   return -1;

}
