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


* CHAPTER_01: **An Introduction to Device Drivers**
    * hello_world001 : first example (just using *printk()* ).
* CHAPTER_02: **Bulding and Debugging Modules**
    * hello_world002 : passing parameters, trying *current->*, using *MODULE_*
    * kdatasize      : a really simple code that shows the size of the types that I can use in a device driver module. The code comes from Alessandro Rubini and Jonathan Corbet (2001 O'Reilly & Associates) under Dual BSD/GPL license.
* CHAPTER_03: **Char Drivers**
    * hello_world003 : using major and minor number and registering the new device driver as a new module
    * hello_world004 : I will try to use (1) file_operation, (2) file and (3) inode (chapter 3 pag 49). Implementing open and close operations (so far completely unuseful but working properly)
    * hello_world005 : Implementing the function read (only a string of 1000*char now)
        * if you try to read more than 1000 char (*DEVICE_MAX_SIZE*), it give you a -1.
        * you can read only and alway the string "MarioBros\0". All the other char characters that you read from the buffer are rubbish.
    * hello_world006 : the same of 005: implementing the same functionality but using *kmalloc* and *kfree*.
    * read_write_dev : in this example, there will be a single chunk of memory that I can read and write. The name of the device is still "hello".
* CHAPTER_04: **Debugging Techniques**
    * .config : the kernelconfiguration used in the host where the drivers where tested
    * debug_01 : using macro to help debugging. The idea and the code come from Alessandro Rubini and Jonathan Corbet (2001 O'Reilly & Associates) and it is explained in the chapter 4 of the book Linux Device Drivers.
        * the Makefile was also changed
            * added make clean
            * added the DEBUG option
        * all the *printk(KERNEL_INFO ...)* were changed to be printed only in the debug mode
    * seq_file_interface : using files in */proc* to communicate information to the user space (debug information or simply information)
        * the idea (and source file) comes from Philippe Reynes under GPL license. It was modified fixing some little problem in order to have a working module for a kernel version > 4.10.0. How to use:
            * Set MAX_LINE_PRINTED to a maximun number of line that you wanto to visulize in /proc/LEO_seq_file (5 in this example)
            * make
            * source proc_load
            * cat /proc/LEO_seq_file
            * observe the output
            * source proc_unload
        * [Manage /proc file with seq_file][2] and a [pdf version][4] of it.
        * [The seq_file interface][3] in the linux source code documentation.
* CHAPTER_05: **Concurrency and Race Conditions**
    * read_write_dev_01 : using semaphore as mutex fot the critical session(read and write sections): *down_interruptible()* and *up()*.
    * complete: using *wait_for_completion_interruptible_timeout(&comp,timeout);*  and *complete(&comp);*
        * in order to test this module you need to use two thread: launch the test_read.elf in a terminal that will lock the component with *wait_for_completion_interruptible_timeout(&comp,timeout)*. The process will wait until another thread (in this case test_write launched in another terminal) frees the semaphore with *complete(&comp)*.
    * spinlocks : TODO



[1]: https://free-electrons.com/doc/books/ldd3.pdf
[2]: https://linux.die.net/lkmpg/x861.html
[3]: https://www.kernel.org/doc/Documentation/filesystems/seq_file.txt
[4]: http://www.tldp.org/LDP/lkmpg/2.6/lkmpg.pdf
