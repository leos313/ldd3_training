 /* hellotest.c
 *
 * Author  :  Leonardo Suriano<leonardo.suriano@live.it>
 *
 * A simple example of a C program to test some of the
 * operations of the "/dev/hello" device (a.k.a "hello0"),
 *
 * To compile the file: gcc test_hello.c -o test_hello.elf
 *
 */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main() {

    int fd, result, len;

    printf("\n-- TEST hello device_driver--\n");
    /* Open operation */
    if ((fd = open("/dev/hello", O_RDONLY)) < 0 ) {
        perror("1. open failed \n");
        goto fail;
    }
		else{
			  printf("file opend\n");
		}

    /* Read operation */

    char a[1000];
    char b[1000];
    //reading a
    printf("string initialized : %s\n",a);
    result = read(fd, (void*)a, 5);
    if ( result != 0 ){
        printf("ERROR read operation: %d byte(s) could NOT be copied\n",result);
        printf("string read : %s\n",a);
        goto fail;
    }
    else{
        printf("read operation executed succesfully\n");
        printf("string read %s \n",a);
    }
    //reading b
    result = read(fd, (void*)b, 20);
    if ( result != 0 ){
        printf("ERROR read operation: %d byte(s) could NOT be copied\n",result);
        printf("string read : %s\n",b);
        goto fail;
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
