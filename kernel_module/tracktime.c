#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kprobes.h>
#include <linux/pid.h>
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
static char asecmd_buffer[ASE_BUFFER_LEN];

static const struct file_operations asepid_proc_fops;

struct proc_dir_entry *ase_parent, *asecmd_entry;

/**
 * Prototypes
 */
static int asepid_proc_open(struct inode *inode, struct file *file);
static int asepid_proc_show(struct seq_file *m, void *v);
static ssize_t asepid_proc_write(struct file *filp, const char __user *buff, size_t len, loff_t *data);
static void asepid_terminate_pid (int code);
static void asepid_action(long pid);


static struct jprobe jprobe_kill_task = {
    .kp = {
        .symbol_name = "do_exit",
    },
    .entry = (kprobe_opcode_t *) asepid_terminate_pid
};

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

static int asepid_proc_show(struct seq_file *m, void *v)
{
    /* Find the corresponding pid_tracker */
    struct task_struct *this_task;
    struct pid *pid_struct;
    pid_struct = find_get_pid((int) current_pid);
    this_task = pid_task(pid_struct, PIDTYPE_PID);
    seq_printf(m, "PID : %lu, utime %lu\n", current_pid, this_task->utime);
    return 0;
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
    /* What to do when user opens  ``/proc/ase/<pid>`` */
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

static void asepid_terminate_pid (int code){
    struct task_struct *this = current;
    int pid = this->pid;
    char* pid_buffer;
    int i = 0;
    /* Verify existence of ``/proc/ase/<pid>`` */
    for(i = 0 ; i < pid_count ; i++)
        {
            if((long int)(pid_array[i]->numbers[0].nr) == pid)
                {
                    pid_buffer=kmalloc(sizeof(char) * 10, GFP_KERNEL); /* Pid should not be must than 10 digits long, ie 1^10 */
                    snprintf(pid_buffer, 10, "/proc/ase/%d", pid);
                    remove_proc_entry(pid_buffer, NULL);
                    printk(KERN_EMERG "ASECMD: %s removed.\n", pid_buffer);
                    return;
                }
        }
    jprobe_return();
    return;
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

        /* Create ``/proc/ase/<pid>`` */
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
    register_jprobe(&jprobe_kill_task);
    printk(KERN_ALERT "plant jprobe at %p, handler addr %p\n",
           jprobe_kill_task.kp.addr, jprobe_kill_task.entry);
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
