
/*
 * proc_hq_dbg.c
 * add by chenhuazhen for hq_debug proc interface
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "internal.h"


static struct proc_dir_entry *proc_hq_debug_dir=NULL;

/*
 * Create hq-dbg Entry.
 */


struct proc_dir_entry * proc_hq_dbg_mkdir(const char *name)
{
	struct proc_dir_entry *proc_entry;
	if(proc_hq_debug_dir==NULL) return NULL;
	proc_entry =proc_mkdir(name,proc_hq_debug_dir);
	if(proc_entry==NULL)
	{
	    printk(KERN_ERR "Create hq-dbg/%s error !",name);
	}
	return 	proc_entry;
}
EXPORT_SYMBOL(proc_hq_dbg_mkdir);

/*
 * Create hq-dbg Entry.
 */
/*
 int proc_hq_dbg_create_entry(const char *name,read_proc_t *read_proc,write_proc_t *write_proc)
{
	struct proc_dir_entry *proc_entry;

	if(proc_hq_debug_dir==NULL) return -1;
	proc_entry =create_proc_entry(name,0766,proc_hq_debug_dir);
	if(proc_entry!=NULL)
	{
	    proc_entry->read_proc=read_proc;	
	    proc_entry->write_proc=write_proc;
	    return 0;
	}
	else
	{
	    printk(KERN_ERR "Create hq-dbg/%s error !",name);
	    return -1;
	}	
}

EXPORT_SYMBOL(proc_hq_dbg_create_entry);
*/
int proc_hq_dbg_add(char * name,struct hq_dbg_entry hq_debug[],int size)
{
	struct proc_dir_entry *proc_dir,*proc_entry;
	int i=0;
	if(proc_hq_debug_dir==NULL) return -1;
	proc_dir =proc_mkdir(name,proc_hq_debug_dir);
	if(proc_dir==NULL)
	{
		printk(KERN_ERR "Create dir  hq-dbg/%s/ error !",name);
		return -1;
	}
	for(i=0;i<size;i++) 
	{
		proc_create(hq_debug[i].name, 0666, proc_dir, &hq_debug[i].proc_operations);
	}
	
	return 0;
}
EXPORT_SYMBOL(proc_hq_dbg_add);

/*
 * Called on initialization to mkdir /proc/hq-dbg/
 */
static int  __init proc_hq_debug_init(void)
{
	proc_hq_debug_dir = proc_mkdir("hq-thermal", NULL);
	if (proc_hq_debug_dir == NULL)
	{
	    printk(KERN_ERR "chz Create hq-dbg/ error !\n");
			return -1;
	}
	return 0;
}
module_init(proc_hq_debug_init);
