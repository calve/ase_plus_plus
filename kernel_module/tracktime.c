#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>



MODULE_AUTHOR("Arthur & Kevin");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ASE Project kernel");


#define ASE_BUFFER_LEN 64
#define PID_LEN 5
#define MAX_PID 1024

long current_pid;

static int pid_count = 0;
static struct pid *pid_array[MAX_PID];

static const struct file_operations asepid_proc_fops;

struct proc_dir_entry *ase_parent, *asecmd_entry;

/**
 * Prototypes
 */
static int asepid_proc_open(struct inode *inode, struct file *file);
static int asepid_proc_show(struct seq_file *m, void *v);
static int init_track_pid(struct file *file, const char *data, size_t size, loff_t *offset);

static const struct file_operations asepid_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = asepid_proc_open,
    .read       = seq_read
/*    .write      = asepid_proc_write,
    .llseek     = seq_lseek,
    .release    = single_release,*/
};

static const struct file_operations asecmd_proc_fops = {
    .owner      = THIS_MODULE,
    .write      = init_track_pid
/*    .open       = asecmd_proc_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,*/
};

/**
 * Now manage files under ``proc/ase/{pid}``
 */

struct asepid_pid {
    unsigned long long start_time;
    unsigned int pid;
    struct asepip_pid *next;
};

struct asepid_pid *pid_list = NULL;

static int asepid_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "PID : %ld\n", current_pid);
    return 0;
}


static int asepid_proc_open(struct inode *inode, struct file *file)
{
    /* What to do when user opens ``ase_cmd`` */
    /*    printk(KERN_INFO "ASEPID: Opened file %s", path_get((const struct path*) file->f_path)); */
    switch(kstrtol(file->f_path.dentry->d_iname, 10, &current_pid)){
    case -ERANGE:
        printk(KERN_EMERG "ASECMD_ERROR: Overflow during conversion.\n");
        return -ERANGE;
    case -EINVAL:
        printk(KERN_EMERG "ASECMD_ERROR: Impossible to convert a string to a long\n");
        return -EINVAL;
    }
    return single_open(file, asepid_proc_show, NULL);
}

/**
 * Manage the tracking of the PID
 */
static void asepid_action(const char *pid_str){
    long pid;
    int i;
    struct pid *pid_struct;

    printk(KERN_EMERG "ASECMD: Entering the add_pid_action function.\n");
    switch(kstrtol(pid_str, 10, &pid)){
    case -ERANGE:
        printk(KERN_EMERG "ASECMD_ERROR: Overflow during conversion.\n");
        return ;
    case -EINVAL:
        printk(KERN_EMERG "ASECMD_ERROR: Impossible to convert a string to a long\n");
        return ;
    }
    if((pid_struct = find_get_pid(pid)) != NULL){
        printk(KERN_EMERG "ASECMD: PID find and added in the directory\n");

        /* On vérifie que le fichier n'existe pas avant de le créer. */
        for(i = 0 ; i < pid_count ; i++)
            {
                /* Warning si il existe. */
                if((long int)(pid_array[i]->numbers[0].nr) == pid)
                    {
                        printk(KERN_EMERG "ASECMD: The PID file already exists.\n");
                        return;
                    }
            }
        proc_create(pid_str, 0644, ase_parent, &asepid_proc_fops);

        pid_array[pid_count] = pid_struct;
        pid_count++;
    }
    else{
        printk(KERN_EMERG "ASECMD: The PID value does not correspond to a existing PID\n");
    }
}


static int init_track_pid(struct file *file, const char __user *buff, size_t size, loff_t *data){
    char *tmp = kmalloc(sizeof(char) * size, GFP_KERNEL);
    int i;

    printk(KERN_EMERG "ASECMD: PID Tracking begin\n");

    if (size > (ASE_BUFFER_LEN - 1)) {
        return -EINVAL;
    }
    for(i = 0; i < size-1; i++){
        tmp[i] = buff[i];
    }
    tmp[i] = '\0';

    asepid_action(tmp);

    return size;
}


/**
 * Manage global module behaviour
 */

static int __init
ase_proc_init(void)
{
    ase_parent = proc_mkdir("ase", NULL);
    if(!ase_parent)
        {
            printk(KERN_INFO "Error creating proc entry\n");
            return -ENOMEM;
        }
    asecmd_entry = proc_create("ase_cmd", 0666, NULL, &asecmd_proc_fops);
    if(!asecmd_entry)
        {
            printk(KERN_INFO "Error creating proc entry\n");
            return -ENOMEM;
        }
    printk(KERN_INFO "Ase initialized\n");
    return 0;
}

static void __exit
ase_proc_exit(void)
{
    remove_proc_entry("ase_cmd", NULL);
    remove_proc_entry("ase", NULL);
}

module_init(ase_proc_init);
module_exit(ase_proc_exit);