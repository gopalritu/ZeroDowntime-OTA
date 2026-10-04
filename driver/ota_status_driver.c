#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "ota_status_ioctl.h"


static struct ota_status_data ota_state = {
    .active_slot = 'A',
    .pending_slot = 'N',
    .status = "IDLE",
    .boot_attempts = 0
};

static DEFINE_MUTEX(ota_lock);


/*
 * Read OTA status from the device
 */
static ssize_t ota_status_read(struct file *file,
                               char __user *buffer,
                               size_t length,
                               loff_t *offset)
{
    char status_buffer[128];
    int size;

    mutex_lock(&ota_lock);

    size = snprintf(
        status_buffer,
        sizeof(status_buffer),
        "ACTIVE_SLOT=%c PENDING_SLOT=%c STATUS=%s BOOT_ATTEMPTS=%d\n",
        ota_state.active_slot,
        ota_state.pending_slot,
        ota_state.status,
        ota_state.boot_attempts
    );

    mutex_unlock(&ota_lock);

    return simple_read_from_buffer(
        buffer,
        length,
        offset,
        status_buffer,
        size
    );
}


/*
 * Handle ioctl commands from user-space
 */
static long ota_status_ioctl(struct file *file,
                             unsigned int command,
                             unsigned long argument)
{
    struct ota_status_data new_state;

    switch (command) {

    case OTA_SET_STATUS:

        if (copy_from_user(
                &new_state,
                (struct ota_status_data __user *)argument,
                sizeof(new_state))) {

            return -EFAULT;
        }

        mutex_lock(&ota_lock);

        ota_state = new_state;

        mutex_unlock(&ota_lock);

        printk(KERN_INFO
               "OTA Driver: status updated - Slot=%c Status=%s Attempts=%d\n",
               ota_state.active_slot,
               ota_state.status,
               ota_state.boot_attempts);

        break;


    case OTA_GET_STATUS:

        mutex_lock(&ota_lock);

        new_state = ota_state;

        mutex_unlock(&ota_lock);

        if (copy_to_user(
                (struct ota_status_data __user *)argument,
                &new_state,
                sizeof(new_state))) {

            return -EFAULT;
        }

        break;


    default:
        return -EINVAL;
    }

    return 0;
}


static const struct file_operations ota_status_fops = {
    .owner = THIS_MODULE,
    .read = ota_status_read,
    .unlocked_ioctl = ota_status_ioctl,
};


static struct miscdevice ota_status_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "ota_status",
    .fops = &ota_status_fops,
    .mode = 0666,
};


/*
 * Driver initialization
 */
static int __init ota_status_init(void)
{
    int result;

    result = misc_register(&ota_status_device);

    if (result != 0) {
        printk(KERN_ERR
               "OTA Driver: failed to register device\n");

        return result;
    }

    printk(KERN_INFO
           "OTA Driver: /dev/ota_status registered\n");

    return 0;
}


/*
 * Driver cleanup
 */
static void __exit ota_status_exit(void)
{
    misc_deregister(&ota_status_device);

    printk(KERN_INFO
           "OTA Driver: /dev/ota_status removed\n");
}


module_init(ota_status_init);
module_exit(ota_status_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ritu Raj");
MODULE_DESCRIPTION(
    "Linux Character Device Driver for ZeroDowntime OTA"
);
MODULE_VERSION("2.0");
