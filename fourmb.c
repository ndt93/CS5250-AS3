#include "fourmb.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "fourmb"

#define BUF_SIZE (4*1024*1024)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int fourmb_open(struct inode *inode, struct file *filep);
int fourmb_release(struct inode *inode, struct file *filep);
ssize_t fourmb_read(struct file *filep, char *buf,
                     size_t count, loff_t *f_pos);
loff_t fourmb_llseek(struct file *file, loff_t offset, int whence);
long fourmb_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
ssize_t fourmb_write(struct file *filep, const char *buf,
                      size_t count, loff_t *f_pos);
static void fourmb_exit(void);
void copy_from_arg(char *to, unsigned long arg);
void copy_to_arg(char *from, unsigned long arg);

struct file_operations fourmb_fops = {
    .read = fourmb_read,
    .write = fourmb_write,
    .llseek = fourmb_llseek,
    .unlocked_ioctl = fourmb_ioctl,
    .open = fourmb_open,
    .release = fourmb_release
};

char *fourmb_data = NULL;
char *fourmb_ptr;
size_t data_len = 0;

char dev_msg[MSG_MAX_LEN + 1];
char tmp_dev_msg[MSG_MAX_LEN + 1];

int fourmb_open(struct inode *inode, struct file *filep)
{
    fourmb_ptr = fourmb_data;
    return 0;
}

int fourmb_release(struct inode *inode, struct file *filep)
{
    return 0;
}

ssize_t fourmb_read(struct file *filep, char *buf,
                    size_t count, loff_t *f_pos)
{
    size_t bytes_read = MIN(count, data_len - (fourmb_ptr - fourmb_data));
    copy_to_user(buf, fourmb_ptr, bytes_read);
    fourmb_ptr += bytes_read;
    return bytes_read;
}

ssize_t fourmb_write(struct file *filep, const char *buf,
                     size_t count, loff_t *f_pos)
{
    size_t bytes_written = MIN(count, BUF_SIZE - (fourmb_ptr - fourmb_data));
    copy_from_user(fourmb_ptr, buf, bytes_written);
    fourmb_ptr += bytes_written;
    data_len = MAX(data_len, fourmb_ptr - fourmb_data);
    return bytes_written;
}

loff_t fourmb_llseek(struct file *file, loff_t offset, int whence)
{
    char *new_ptr;

    switch (whence) {
        case SEEK_SET:
            new_ptr = fourmb_data + offset;
            break;
        case SEEK_CUR:
            new_ptr = fourmb_ptr + offset;
            break;
        case SEEK_END:
            new_ptr = fourmb_data + data_len + offset;
            break;
        default:
            new_ptr = fourmb_ptr;
    }

    if (new_ptr < fourmb_data) {
        new_ptr = fourmb_data;
    } else if (new_ptr > fourmb_data + data_len) {
        new_ptr = fourmb_data + data_len;
    }

    fourmb_ptr = new_ptr;
    return fourmb_ptr - fourmb_data;
}

void copy_from_arg(char *to, unsigned long arg)
{
    int bytes_copied = 0;
    char *c = (char *)arg;
    while (bytes_copied < MSG_MAX_LEN && c) {
        get_user(*(to++), c++);
        bytes_copied++;
    }
    *to = '\0';
}

void copy_to_arg(char *from, unsigned long arg)
{
    int bytes_copied = 0;
    char *c = (char *)arg;
    while (bytes_copied < MSG_MAX_LEN && c) {
        put_user(*(from++), c++);
        bytes_copied++;
    }
    put_user('\0', c);
}

long fourmb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case IOCTL_GET_MSG:
            copy_to_arg(dev_msg, arg);
            break;
        case IOCTL_SET_MSG:
            copy_from_arg(dev_msg, arg);
            break;
        case IOCTL_SWP_MSG:
            memcpy(tmp_dev_msg, dev_msg, sizeof(dev_msg));
            copy_from_arg(dev_msg, arg);
            copy_to_arg(tmp_dev_msg, arg);
            printk(KERN_ALERT "dev_msg: %s", dev_msg);
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

static int fourmb_init(void)
{
    int result;

    result = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fourmb_fops);
    if (result < 0) {
        return result;
    }

    fourmb_data = kmalloc(BUF_SIZE, GFP_KERNEL);
    if (!fourmb_data) {
        fourmb_exit();
        return -ENOMEM;
    }
    data_len = 0;

    printk(KERN_ALERT "This is a fourmb device module\n");
    return 0;
}

static void fourmb_exit(void)
{
    if (fourmb_data) {
        kfree(fourmb_data);
        fourmb_data = NULL;
    }
    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
    printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(fourmb_init);
module_exit(fourmb_exit);
