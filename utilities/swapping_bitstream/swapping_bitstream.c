/*****************************************************************************/
/**
*
* Swap bitstream
* Leonardo Suriano
*
*
*
* File designed to swap the bitsteam for exploiting the Dynamic Partial Reconfiguration.
* The file was designed using the MORA's code for swapping
*
*
*
*
*
*
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

void prepare_pbs(void *addr, size_t size) {

	// Local variables
	unsigned int i;
	size_t size_gc = size;

	// Check if memory contains valid bitstream file
	if ((size % 4) != 0) {
		printf("WARNING: bitstream file might be corrupted, size is not multiple of 4 (32-bit words)\n");
	}

	// Read 4 bytes, swap them
	volatile uint32_t *aux = addr;
	printf("Preparing 32-bit data @ %p...\n", addr);
	for (i = 0; i < size/4; i++) {
		if (size_gc != size )
			printf("something has happened with size\n");
		uint32_t value = *aux;
		value = ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
		*aux = value;
		aux++;
	}
}

int main(){
	printf("...starting swapping bitstream\n");
	FILE *fp;
	fp = fopen("design_1_wrapper.bit","rb");
	if (fp == NULL) {
        printf ("File not created okay, errno = %d\n", errno);
        return 1;
    }
    else {
		printf("OK opening binary file file\n");
	}
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);  //same as rewind(f);
	printf("fsize = %zu bytes\n", fsize);
	char *string = malloc(fsize + 1);
	size_t ret = fread(string, fsize, 1, fp);
	//printf("read %zu bytes\n", ret*sizeof(*string));
	prepare_pbs((void *)string, (size_t) fsize);
	fclose(fp);

	////writing back bitstream

	FILE *fw;
	//char *bitstream_swapped = malloc(fsize + 1);

	fw = fopen("design_SWAPPED.bit","wb");
	if (fw == NULL) {
        printf ("File not created okay, errno = %d\n", errno);
        return 1;
    }
    else {
		printf("OK opening binary file file\n");
	}
	fwrite(string, fsize, 1, fw);

	fclose(fw);


	printf("DONE\n");
	return 0;
}
