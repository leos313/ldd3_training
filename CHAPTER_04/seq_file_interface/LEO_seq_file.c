/*
* the idea (and source file) comes from Philippe Reynes under GPL license.
* It was modified in order to have a working module for a
* kernel version > 4.10.0
*
* How to use:
* - Set MAX_LINE_PRINTED to a maximun number of line that you wanto to visulize in /proc/LEO_seq_file (5 in this example)
* - make
* - source proc_load
* - cat /proc/LEO_seq_file
*          Hello Leo in proc_seq_file 0
*          Hello Leo in proc_seq_file 1
*          Hello Leo in proc_seq_file 2
*          Hello Leo in proc_seq_file 3
*          Hello Leo in proc_seq_file 4
* - source proc_unload
*
*/


/**
 *  procfs4.c -  create a "file" in /proc
 * 	This program uses the seq_file API to manage the /proc file.
 *
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/proc_fs.h>	/* Necessary because we use proc fs */
#include <linux/seq_file.h>	/* for seq_file */

#include "LEO_seq_file.h" /* debug macros  */

#define PROC_NAME	"LEO_seq_file"

MODULE_AUTHOR("Leonardo Suriano <leonardo.suriano@live.it>");
MODULE_LICENSE("GPL");

/**
 * This function is called at the beginning of a sequence.
 * ie, when:
 *	- the /proc file is read (first time)
 *	- after the function stop (end of sequence)
 *
 */
static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long counter = 0;

	/* beginning a new sequence ? */
  PDEBUG("[START]: *pos = %llu, counter = %lu \n",*pos,counter);
	if ( *pos < MAX_LINE_PRINTED )
	{
		/* yes => return a non null value to begin the sequence */
		return &counter;
	}
	else
	{
		/* no => it's the end of the sequence, return end to stop reading */
		*pos = 0;
		return NULL;
	}
}

/**
 * This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 *
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *)v;
	(*tmp_v)++;
	PDEBUG("[NEXT] : BEFORE *pos = %llu \n",*pos);
	// (*pos)++;
	// (*pos)++;
	// (*pos)++;
  PDEBUG("[NEXT] : AFTER  *pos = %llu \n",*pos);
	return NULL;
}

/**
 * This function is called at the end of a sequence
 *
 */
static void my_seq_stop(struct seq_file *s, void *v)
{
	/* nothing to do, we use a static value in start() */
  //loff_t *spos = (loff_t *) v;
  //PDEBUG("[SHOW]: *v = *spos = %Ld \n",*spos);
}

/**
 * This function is called for each "step" of a sequence
 *
 */
static int my_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
  seq_printf(s, "Hello Leo in proc_seq_file %Ld\n", *spos);
  PDEBUG("[SHOW] : *v = *spos = %Ld \n",*spos);
	return 0;
}

/**
 * This structure gather "function" to manage the sequence
 *
 */
static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next  = my_seq_next,
	.stop  = my_seq_stop,
	.show  = my_seq_show
};

/**
 * This function is called when the /proc file is open.
 *
 */
static int my_open(struct inode *inode, struct file *file)
{
  PDEBUG("opening proc/ file \n");
	return seq_open(file, &my_seq_ops);
};

/**
 * This structure gather "function" that manage the /proc file
 *
 */
static struct file_operations my_file_ops = {
	.owner   = THIS_MODULE,
	.open    = my_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};


/**
 * This function is called when the module is loaded
 *
 */
int init_module(void)
{
	struct proc_dir_entry *entry;
  PDEBUG("[init_module] int module function \n");
	entry = proc_create(PROC_NAME, 0, NULL,&my_file_ops);
  if(entry == NULL)
      return -ENOMEM;

  return 0;
}

/**
 * This function is called when the module is unloaded.
 *
 */
void cleanup_module(void)
{
	remove_proc_entry(PROC_NAME, NULL);
}
