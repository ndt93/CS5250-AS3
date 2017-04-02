#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61

/* Forward declaration */
int fourmb_open(struct inode *inode, struct file *filep);
int fourmb_release(struct inode *inode, struct file *filep);
ssize_t fourmb_read(struct file *filep, char *buf,
                     size_t count, loff_t *f_pos);
ssize_t fourmb_write(struct file *filep, const char *buf,
                      size_t count, loff_t *f_pos);
static void fourmb_exit(void);

/* Definition of file_operation structure */
struct file_operations fourmb_fops = {
    read:     fourmb_read,
    write:    fourmb_write,
    open:     fourmb_open,
    release:  fourmb_release
};

char *fourmb_data = NULL;
char *fourmb_ptr;

int fourmb_open(struct inode *inode, struct file *filep)
{
    fourmb_ptr = fourmb_data;
    return 0;  // Always successful
}

int fourmb_release(struct inode *inode, struct file *filep)
{
    return 0;  // Always successful
}

ssize_t fourmb_read(struct file *filep, char *buf,
                     size_t count, loff_t *f_pos)
{
    if (!fourmb_ptr)
        return 0;

    if (count == 0)
        return 0;

    put_user(*fourmb_ptr, buf);
    fourmb_ptr = NULL;
    return 1;
}

ssize_t fourmb_write(struct file *filep, const char *buf,
                      size_t count, loff_t *f_pos)
{
    if (count == 0)
        return 0;

    get_user(*fourmb_data, buf);

    if (count > 1)
        return -ENOSPC;
    return 1;
}

static int fourmb_init(void)
{
    int result;
    // Register the device
    result = register_chrdev(MAJOR_NUMBER, "fourmb", &fourmb_fops);
    if (result < 0) {
        return result;
    }

    /* Allocate one byte of memory for storage
     * kmalloc is just like malloc, the second parameter is
     * the type of memory to be allocated.
     * To release the memory allocated by kmalloc, use kfree. */
    fourmb_data = kmalloc(sizeof(char), GFP_KERNEL);
    if (!fourmb_data) {
        fourmb_exit();
        // Cannot allocate memory
        // Return no memory error, negative signify a failure
        return -ENOMEM;
    }
    // Initialize the value to be X
    *fourmb_data = 'X';
    printk(KERN_ALERT "This is a fourmb device module\n");
    return 0;
}

static void fourmb_exit(void)
{
    // If the pointer is pointing to something
    if (fourmb_data) {
        // Free the memory and assign the pointer to NULL
        kfree(fourmb_data);
        fourmb_data = NULL;
    }
    // Unregister the device
    unregister_chrdev(MAJOR_NUMBER, "fourmb");
    printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(fourmb_init);
module_exit(fourmb_exit);
