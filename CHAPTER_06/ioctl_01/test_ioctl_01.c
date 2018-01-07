 /* ioctl_01test.c
 *
 * Author  :  Leonardo Suriano<leonardo.suriano@live.it>
 *
 * A simple example of a C program to test some of the
 * operations of the "/dev/ioctl_01" device (a.k.a "ioctl_010"),
 *
 * To compile the file: gcc -O -g -DLEO_DEBUG test_ioctl_01.c -o test_ioctl_01.elf
 *
 */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "test_ioctl_01.h"

/* Use 'k' as magic number */
#define IOCTL_01_IOC_MAGIC  'T'
/* Please use a different 8-bit number in your code */

#define DEVICE_IOCRESET    _IO(IOCTL_01_IOC_MAGIC, 0)
#define SET_FIRST_BUFFER   _IO(IOCTL_01_IOC_MAGIC, 1)
#define SET_SECOND_BUFFER  _IO(IOCTL_01_IOC_MAGIC, 2)
#define WHICH_BUFFER       _IOR(IOCTL_01_IOC_MAGIC, 3, int)

int main() {

    int fd, result;

    printf("\n-- TEST ioctl_01 device_driver--\n");
    PDEBUG(" DEVICE_IOCRESET:    %u\n",DEVICE_IOCRESET);
    PDEBUG(" SET_FIRST_BUFFER:   %u\n",SET_FIRST_BUFFER);
    PDEBUG(" SET_SECOND_BUFFER:  %u\n",SET_SECOND_BUFFER);
    PDEBUG(" WHICH_BUFFER:       %lu\n",WHICH_BUFFER);
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

    /* using ioctl */

    int value_ioctl = ioctl(fd,SET_FIRST_BUFFER);
    printf("value_ioctl : %d\n", value_ioctl);
    printf(" %s\n", strerror(errno));

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
