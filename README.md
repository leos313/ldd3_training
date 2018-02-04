# ldd3 Training -Linux Device Drivers-

## Introduction

I am writing these sample codes for a **personal reason**: I really want to learn how to write (correctly!) Linux device drivers.

In order to achieve this goal, the best source/documentation is, of course, the famous [Linux Device Drivers 3rd edition][1] (a.k.a *ldd3*) by Jonathan Corbet, Alessandro Rubini, and Greg Kroah-Hartman. Reading the book (normally every engineering book) is not enough to deeply understand the topic: the best way is almost always to directly try what you read. This is exactly what I am doing: step by step I try to implement the new acquired concepts.

The folders are divided per chapter and, in every one of them, there should be at least one example where the newly learned techniques were applied. I am sure the code it is not so clean, can be improved, it is not optimized and so on and so forth: I will try to improve my coding skills with these exercises.

* If you are reading this document and you want to try to compile and run these examples, please feel free to do it. But keep in mind that **No warranty is attached**; I cannot take responsibility for errors or fitness for use.
* If you find some errors and you want to help to improve this code, feel free to text me or start issues, pull requests etc.
* If you think there is something wrong or ambiguous with License please text me and I will fix the errors (the examples are "under construction").
* If you want to learn how to write a device driver, please read the [book][1]. You can use the following list of exercises to implement the new commands and strategies and you can compare with the solution I am proposing. Giving me feedback, we can improve, **both**, our codes.



## Examples


1. CHAPTER_01: **An Introduction to Device Drivers**
    1. hello_world001 : first example (just using *printk()* ).
2. CHAPTER_02: **Bulding and Running Modules**
    1. hello_world002 : passing parameters, trying *current->*, using *MODULE_*
    2. kdatasize      : a really simple code that shows the size of the types that I can use in a device driver module. The code comes from Alessandro Rubini and Jonathan Corbet (2001 O'Reilly & Associates) under Dual BSD/GPL license.
3. CHAPTER_03: **Char Drivers**
    1. hello_world003 : using major and minor number and registering the new device driver as a new module
    2. hello_world004 : I will try to use (1) file_operation, (2) file and (3) inode (chapter 3 pag 49). Implementing open and close operations (so far completely unuseful but working properly)
    3. hello_world005 : Implementing the function read (only a string of 1000*char now)
        * if you try to read more than 1000 char (*DEVICE_MAX_SIZE*), it give you a -1.
        * you can read only and alway the string "MarioBros\0". All the other char characters that you read from the buffer are rubbish.
    4. hello_world006 : the same of 005: implementing the same functionality but using *kmalloc* and *kfree*.
    5. read_write_dev : in this example, there will be a single chunk of memory that I can read and write. The name of the device is still "hello".
4. CHAPTER_04: **Debugging Techniques**
    1. .config : the kernelconfiguration used in the host where the drivers where tested
    2. debug_01 : using macro to help debugging. The idea and the code come from Alessandro Rubini and Jonathan Corbet (2001 O'Reilly & Associates) and it is explained in the chapter 4 of the book Linux Device Drivers.
        * the Makefile was also changed
            * added make clean
            * added the DEBUG option
        * all the *printk(KERNEL_INFO ...)* were changed to be printed only in the debug mode
    3. seq_file_interface : using files in */proc* to communicate information to the user space (debug information or simply information)
        * the idea (and source file) comes from Philippe Reynes under GPL license. It was modified fixing some little problem in order to have a working module for a kernel version > 4.10.0. How to use:
            * Set MAX_LINE_PRINTED to a maximun number of line that you wanto to visulize in /proc/LEO_seq_file (5 in this example)
            * make
            * source proc_load
            * cat /proc/LEO_seq_file
            * observe the output
            * source proc_unload
        * How it works and big part of the source file come from:
            * [Manage /proc file with seq_file][2] and a [pdf version][4] of it.
            * [The seq_file interface][3] in the linux source code documentation.
        * Note that the value pointed by *pos* is incremented automatically at the end of the function *stop* and it re-start from zero every time you read again the */proc/LEO_seq_file*. You can do everything with the value pointed by *pos* but, when a new iteration starts, it is **ONLY** automatically incremented by 1 by the API. Instead, the value *counter* is a global variable (because of *static*) and it holds its value until you remove the module from the kernel.
5. CHAPTER_05: **Concurrency and Race Conditions**
    1. read_write_dev_01 : using semaphore as mutex fot the critical session(read and write sections): *down_interruptible()* and *up()*.
    2. complete: using *wait_for_completion_interruptible_timeout(&comp,timeout);*  and *complete(&comp);*
        * in order to test this module you need to use two thread: launch the test_read.elf in a terminal that will lock the component with *wait_for_completion_interruptible_timeout(&comp,timeout)*. The process will wait until another thread (in this case test_write launched in another terminal) frees the semaphore with *complete(&comp)*.
    3. read_write_dev_02 : the same of *read_write_dev_01*. Here I am going to add:
        * the debug macros
        * updating Makefile
        * the seq_file_interface (with semaphores in the function *show*)
        * using API for printing device number
    4. spinlocks : TODO
6. CHAPTER_06: **Advanced Char Driver Operations**
    1. ioctl_01: in this first example I will try to implement some command in a device drivers. The goal will be to change read/write buffer choosing between two.
        * command implemented and tested (using semaphore because of shared/global variable)
        * read info from kernel-space through ioctl(...)
        * you can use two different buffer to store your data-string
        * the test_ioctl_01.c read and write, alternatively, from the two buffer. When NO buffers are set up, read and write are not permitted (see the kernel messages with *dmesg*).
        * a single semaphore was used for both buffers (this can have an impact on the performance). It is possible to design a module that uses two semaphores/mutexs: one for each buffers, making them independend from each other.
7. DEVICE_TREE: *Managing Device Tree*
    1. devicetree_helloworld01: in this example I use the same source file of CHAPTER_03 -> hello_world003 where I add some basic function to read the device tree (in this case for the Pynq board) and print the address of the gpio found.
    2. *managing_leds*:
        * this driver turns on the first 3 leds (corrisponding to the binary value 7 -> 0b0111) and uses ioremap and iowrite32(...)
        * TODO: to use ioctl(...) to turn the led on and off design the driver in a proper manner.
8. utilities:
    1. swapping_bitstream: simple program to swap bitstream (the function was originally written by MORA and modify by me for this project)
    2. test_gpio_userspace: using mmap I can access directly to the hardware (with NO device driver). Within the C-file there are the right command to turn on/off the Leds on Pynq board and to read the button/switch of the board. It assumes that you have created a Vivado project with GPIO with the right address map. See the [blog's page](http://fpga.org/2013/05/28/how-to-design-and-access-a-memory-mapped-device-part-two/)
9. CHAPTER_09: **Communicating with Hardware**
    1. LED_01: the aim of this driver will be to turn on/off a LED using *ioctl(...)* system call.
        * Just compile the test files ON.c, OFF.c and QUERY.c to test the device driver. With this TEST you will use the *ioctl(..)* system call
        * If you compile and use BLINDING.c you will use the system call *write(...)* to turn on and off the led. The function *read()* is implemented but does nothing useful so far.
        * For getting the resource, please read [this link](http://xillybus.com/tutorials/device-tree-zynq-4).
            * added the function `resource_size(...)` instead of inserting manually the size of the memory area.
10. CHAPTER_10: **Interrupt Handling**
    * cat /proc/interrupts
    * cat /proc/stat


[1]: https://free-electrons.com/doc/books/ldd3.pdf
[2]: https://linux.die.net/lkmpg/x861.html
[3]: https://www.kernel.org/doc/Documentation/filesystems/seq_file.txt
[4]: http://www.tldp.org/LDP/lkmpg/2.6/lkmpg.pdf
