# Comment/uncomment the following line to disable/enable debugging

# This Makefile is designed to crosscompile the module to be loded onto a Pynq board running a Linaro/Linux 4.9.0
#
# source /opt/Xilinx/SDK/2017.1/settings64.sh
# export KERNELDIR=/home/embedded/zynq/repos/linux-xlnx/
# export KERNELDIR=/home/leonardo/Documents/LDD3/pynq_OS/linux-xlnx/
# export ARCH=arm
# export CROSS_COMPILE=/opt/Xilinx/SDK/2017.1/gnu/arm/lin/bin/arm-xilinx-linux-gnueabi-


DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DLEO_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

LDDINC=$(PWD)/../include

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

#proc_seq_file-objs := main.o pipe.o access.o

obj-m	:= LED_01.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.elf *.order *.symvers

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
