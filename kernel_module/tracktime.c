#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#define ASE_BUFFER_LEN 5
#define PID_LEN 5
static char asecmd_buffer[ASE_BUFFER_LEN];
struct proc_dir_entry *ase_parent, *asecmd_entry;


static void asepid_proc_create(int pid);
static const struct file_operations asepid_proc_fops;

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
    kstrtol(asecmd_buffer, 0, &res);
    asecmd_buffer[len] = 0;
    asepid_proc_create(res);
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
    
static void asepid_proc_create(int pid){
    char procname[PID_LEN];
    int status;
    struct asepid_pid *this = vmalloc(sizeof(struct asepid_pid));
    printk(KERN_INFO "ASECMD: will track PID");
    status = snprintf(procname, PID_LEN, "%i", pid);

    /* Check pid existance */
    this->pid = pid;
    this->start_time = (unsigned long long) get_jiffies_64();
    if (pid_list == NULL) {
        pid_list = this;
    }
    proc_create(procname, 0666, ase_parent, &asepid_proc_fops);
    printk(KERN_INFO "ASECMD: begin PID tracking");
}

static int
asepid_proc_show(struct seq_file *m, void *v)
{
    return 0;
}


static int
asepid_proc_open(struct inode *inode, struct file *file)
{
    /* What to do when user opens ``ase_cmd`` */
    /*    printk(KERN_INFO "ASEPID: Opened file %s", path_get((const struct path*) file->f_path)); */
    return single_open(file, asepid_proc_show, NULL);
}

static ssize_t
asepid_proc_write(struct file *filp, const char __user *buff,
               size_t len, loff_t *data)
{
    /* What to do when user writes to ``ase/{pid}`` */
    printk(KERN_INFO "ASEPID: Write has been called");
    return len;
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
 * Manage global module behaviour
 */

static int __init
ase_proc_init(void)
{
    ase_parent = proc_mkdir("ase", NULL);
    if(!ase_parent)
        {
            printk(KERN_INFO "Error creating proc entry");
            return -ENOMEM;
        }
    asecmd_entry = proc_create("ase_cmd", 0666, NULL, &asecmd_proc_fops);
    if(!asecmd_entry)
        {
            printk(KERN_INFO "Error creating proc entry");
            return -ENOMEM;
        }
    printk(KERN_INFO "Ase initialized");
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

MODULE_AUTHOR("Arthur");
MODULE_LICENSE("WTFPL");
MODULE_DESCRIPTION("ASE Project");
