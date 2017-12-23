#ifndef _HELLO_H_
#define _HELLO_H_

#define DEVICE_MAX_SIZE (20)




struct hello_dev {
	char *p_data;              /* pointer to the memory allocated */
	struct cdev cdev;	         /* Char device structure		*/
};






#endif /* _HELLO_H_ */
