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
#define MAX_PID 8192

long current_pid;

static int pid_count = 0;
static struct pid *pid_array[MAX_PID];
static char asecmd_buffer[ASE_BUFFER_LEN];

static const struct file_operations asepid_proc_fops;

struct proc_dir_entry *ase_parent, *asecmd_entry;

/**
 * Prototypes
 */
static int asepid_proc_open(struct inode *inode, struct file *file);
static int asepid_proc_show(struct seq_file *m, void *v);
static ssize_t asepid_proc_write(struct file *filp, const char __user *buff, size_t len, loff_t *data);
static void asepid_action(long pid);
extern struct task_struct *find_task_by_vpid(pid_t nr);


/**
 * Manage ``proc/ase_cmd`` file
 */

static int
asecmd_proc_show(struct seq_file *m, void *v)
{
    return 0;
}


static int
asecmd_proc_open(struct inode *inode, struct file *file)
{
    /* What to do when user opens ``ase_cmd`` */
    return single_open(file, asecmd_proc_show, NULL);
}

static ssize_t
asecmd_proc_write(struct file *filp, const char __user *buff,
               size_t len, loff_t *data)
{
    /* What to do when user writes to ``ase_cmd`` */
    long res;
    printk(KERN_INFO "ASECMD: Write has been called");
    if (len > (ASE_BUFFER_LEN - 1)) {
        printk(KERN_INFO "ASECMD: error, input too long");
        return -EINVAL;
    }
    else if (copy_from_user(asecmd_buffer, buff, len)) {
        return -2;
    }
    asecmd_buffer[len] = 0;
    switch(kstrtol(asecmd_buffer, 0, &res)){
    case -ERANGE:
        printk(KERN_EMERG "ASECMD_ERROR: Overflow during conversion.\n");
        return -1;
    case -EINVAL:
        printk(KERN_EMERG "ASECMD_ERROR: Impossible to convert a string to a long\n");
        return -1;
    }
    asepid_action(res);
    return len;
}


static const struct file_operations asecmd_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = asecmd_proc_open,
    .read       = seq_read,
    .write      = asecmd_proc_write,
    .llseek     = seq_lseek,
    .release    = single_release,
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
    //struct task_struct *task = current; // getting global current pointer
    struct pid *pid_struct;
    struct task_struct *task;
    int i;
    char procname[5];
    char path[9];

    printk(KERN_NOTICE "%ld\n", current_pid);
    /* On vérifie que le fichier n'existe pas avant de le créer. */
    for(i = 0 ; i < pid_count ; i++){
            /* Warning si il existe. */
            if((pid_struct = find_get_pid(current_pid)) != NULL){
                    task = pid_task(pid_struct,PIDTYPE_PID);
                    printk(KERN_NOTICE "assignment: current process: %s, PID: %d, start time:%ld\n", task->comm, task->pid, task->utime);
                    return 0;
            }
    }
    snprintf(procname, 5, "%li", current_pid);
    printk(KERN_NOTICE "%s\n", procname);
    strcat(path, "ase/");
    strcat(path, procname);
    printk(KERN_NOTICE "%s\n", path);
    /* NOT WORKING BUT ALMOST */
    remove_proc_entry(path, NULL);
    printk(KERN_NOTICE "ASECMD_ERROR: Impossible to display the file (not existing)\n");
    return 1;
}

static ssize_t
asepid_proc_write(struct file *filp, const char __user *buff,
               size_t len, loff_t *data)
{
    /* What to do when user writes to ``ase/{pid}`` */
    printk(KERN_INFO "ASEPID: Write has been called");
    return len;
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


static const struct file_operations asepid_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = asepid_proc_open,
    .read       = seq_read,
    .write      = asepid_proc_write,
    .llseek     = seq_lseek,
    .release    = single_release,
};

/**
 * Manage the tracking of the PID
 */
static void asepid_action(long pid){
    int i;
    struct pid *pid_struct;

    printk(KERN_EMERG "ASECMD: Entering the add_pid_action function.\n");
    if((pid_struct = find_get_pid(pid)) != NULL){
        char* pid_buffer;
        pid_buffer=kmalloc(sizeof(char) * 10, GFP_KERNEL); /* Pid should not be must than 10 digits long, ie 1^10 */
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
        snprintf(pid_buffer, 10, "%lu", (long) pid);
        proc_create(pid_buffer, 0644, ase_parent, &asepid_proc_fops);

        pid_array[pid_count] = pid_struct;
        pid_count++;
    }
    else{
        printk(KERN_EMERG "ASECMD: The PID value does not correspond to a existing PID\n");
    }
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
