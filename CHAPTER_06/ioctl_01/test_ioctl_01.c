 /* ioctl_01test.c
 *
 * Author  :  Leonardo Suriano<leonardo.suriano@live.it>
 *
 * A simple example of a C program to test some of the
 * operations of the "/dev/ioctl_01" device (a.k.a "ioctl_010"),
 *
 * To compile the file: gcc test_ioctl_01.c -o test_ioctl_01.elf
 *
 */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main() {

    int fd, result;

    printf("\n-- TEST ioctl_01 device_driver--\n");
    /* Open operation */
    if ((fd = open("/dev/ioctl_01", O_RDWR)) < 0 ) {
        perror("1. open failed \n");
        goto fail;
    }
		else{
			  printf("file opend\n");
		}

    /* Write operation */

    char w_b[12];
    strcpy(w_b,"MarioBros3");
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
    result = read(fd, (void*)a, 3);
    if ( result != 0 ){
        printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
    }
    else{
        printf("read operation executed succesfully\n");
        printf("string read %s \n",a);
    }
    //reading b
    result = read(fd, (void*)b, 7);
    if ( result != 0 ){
        printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
    }
    else{
        printf("read operation executed succesfully\n");
        printf("string read %s \n",b);
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
