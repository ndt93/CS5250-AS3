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
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf,
                     size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf,
                      size_t count, loff_t *f_pos);
static void onebyte_exit(void);

/* Definition of file_operation structure */
struct file_operations onebyte_fops = {
    read:     onebyte_read,
    write:    onebyte_write,
    open:     onebyte_open,
    release:  onebyte_release
};

char *onebyte_data = NULL;
char *onebyte_ptr;

int onebyte_open(struct inode *inode, struct file *filep)
{
    onebyte_ptr = onebyte_data;
    return 0;  // Always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
    return 0;  // Always successful
}

ssize_t onebyte_read(struct file *filep, char *buf,
                     size_t count, loff_t *f_pos)
{
    if (!onebyte_ptr)
        return 0;

    if (count == 0)
        return 0;

    put_user(*onebyte_ptr, buf);
    onebyte_ptr = NULL;
    return 1;
}

ssize_t onebyte_write(struct file *filep, const char *buf,
                      size_t count, loff_t *f_pos)
{
    if (count == 0)
        return 0;

    get_user(*onebyte_data, buf);

    if (count > 1)
        return -ENOSPC;
    return 1;
}

static int onebyte_init(void)
{
    int result;
    // Register the device
    result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
    if (result < 0) {
        return result;
    }

    /* Allocate one byte of memory for storage
     * kmalloc is just like malloc, the second parameter is
     * the type of memory to be allocated.
     * To release the memory allocated by kmalloc, use kfree. */
    onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);
    if (!onebyte_data) {
        onebyte_exit();
        // Cannot allocate memory
        // Return no memory error, negative signify a failure
        return -ENOMEM;
    }
    // Initialize the value to be X
    *onebyte_data = 'X';
    printk(KERN_ALERT "This is a onebyte device module\n");
    return 0;
}

static void onebyte_exit(void)
{
    // If the pointer is pointing to something
    if (onebyte_data) {
        // Free the memory and assign the pointer to NULL
        kfree(onebyte_data);
        onebyte_data = NULL;
    }
    // Unregister the device
    unregister_chrdev(MAJOR_NUMBER, "onebyte");
    printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
