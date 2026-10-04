#ifndef OTA_STATUS_IOCTL_H
#define OTA_STATUS_IOCTL_H

#include <linux/ioctl.h>

#define OTA_IOC_MAGIC 'o'

struct ota_status_data {
    char active_slot;
    char pending_slot;
    char status[32];
    int boot_attempts;
};

#define OTA_SET_STATUS _IOW(OTA_IOC_MAGIC, 1, struct ota_status_data)
#define OTA_GET_STATUS _IOR(OTA_IOC_MAGIC, 2, struct ota_status_data)

#endif
