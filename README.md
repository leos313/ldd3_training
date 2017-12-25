# ldd3 Training
===


* CHAPTER_01
    * hello_world001 : first example (just using printk).
* CHAPTER_02
    * hello_world002 : passing parameters, trying current->, using MODULE_
* CHAPTER_03
    * hello_world003 : using major and minor number and registering the new device driver as a new module
    * hello_world004 : I will try to use (1) file_operation, (2) file and (3) inode (chapter 3 pag 49). Implementing open and close operations (so far compleatly unseful but working properly)
    * hello_world005 : Implementing the function read (only a string of 1000*char now)
        * if you try to read more than 1000 char (DEVICE_MAX_SIZE), it give you a -1.
        * you can read only and alway the string "MarioBros\0". All the other char that you read from the buffer are rubbish.
    * hello_world006 : the same of 005: implementing the same functionality but using kmalloc and kfree.
    * read_write_dev : in this example, there will be a single chunk of memory that I can read and write. The name of the device istill "hello".
