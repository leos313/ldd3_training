 /* hellotest.c
 *
 * Author  :  Leonardo Suriano<leonardo.suriano@live.it>
 *
 * A simple example of a C program to test some of the
 * operations of the "/dev/hello" device (a.k.a "hello0"),
 *
 */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main() {

    int fd, result, len;

    printf("\n-- TEST hello device_driver--\n");
    if ((fd = open("/dev/hello", O_RDONLY)) < 0 ) {
        perror("1. open failed \n");
        return -1;
    }
		else{
			  printf("file opend\n");
		}
    if (close(fd)){
		    perror("1. close failed \n");
		}
		else{
		    printf("file closed\n");
		}
		printf("-- TEST PASSED --\n");
    return 0;

}
