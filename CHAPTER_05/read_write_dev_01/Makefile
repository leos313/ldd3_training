#
# Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
#
# Author: Leonardo Suriano
#
#
# This Makefile is designed to crosscompile the module to be loded onto Ultrascale+ running a Linaro/Linux 4.9.0
#
# source /opt/Xilinx/SDK/2017.2/settings64.sh
# export KERNELDIR=/home/leonardo/Desktop/Ultrascale_prj/Debian_release_src_2017_2/linux-xlnx/
# export ARCH=arm64
# export CROSS_COMPILE=aarch64-linux-gnu-

#CC = $(CROSS_COMPILE)gcc

# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD  := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules -Wall
endif
