# ldd3 Training
===


* CHAPTER_01
    * hello_world001 : first example (just using printk).
* CHAPTER_02
    * hello_world002 : passing parameters, trying current->, using MODULE_
* CHAPTER_03
    * hello_world003 : using major and minor number and registering the new device driver as a new module
    * hello_world004 : I will try to use (1) file_operation, (2) file and (3) inode (chapter 3 pag 49). Implementing open and close operations (so far compleatly unseful but working properly)
    * hello_world005 : Implementing the function read and write (for only a "int" for example) using the function copy_to_user(...) and copy_from_user(...)
