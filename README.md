# ZeroDowntime OTA

### A/B Slot Based Rollback-Safe Embedded Linux OTA Update System

## About

This project is a simple software-based OTA update system for Embedded Linux.

The main idea is to use two slots, **A and B**, for software updates. One slot is active and the other slot is used for the new update.

If the new version works properly, it becomes active. If it fails, the system goes back to the previous working version.

I also implemented a Linux character device driver to show the OTA status using `/dev/ota_status`.

The update process was also tested using a **watch interface**, where different watch versions were used to represent different software versions.

---

## How It Works

```text
        OTA Manager
             |
             ↓
       Inactive Slot
             |
             ↓
       Boot Manager
             |
             ↓
       Watch Interface
             |
             ↓
       Health Check
        /        \
       ↓          ↓
     PASS        FAIL
       |           |
       ↓           ↓
    SUCCESS     ROLLBACK
```

For testing, I used different watch versions:

```text
Watch v1 → Initial version
Watch v2 → Successful update
Watch v3 → Failed update / rollback testing
```

Example:

```text
Slot A → Watch v1
Slot B → Watch v2
```

After a successful update:

```text
Slot B → Watch v2 (Active)
```

For rollback testing, Watch v3 was intentionally treated as a failed version, so the previous working watch version remained active.

---

## Project Structure

```text
ZeroDowntimeOTA/
├── device/
│   ├── app/
│   │   ├── watch_v1.cpp
│   │   ├── watch_v2.cpp
│   │   └── watch_v3.cpp
│   ├── ota/
│   ├── slot_a/
│   ├── slot_b/
│   └── state/
│
├── driver/
├── scripts/
├── updates/
├── images/
├── server/
├── tests/
└── README.md
```
[<img src="https://github.com/gopalritu/ZeroDowntime-OTA/blob/main/images/Project%20Structure/1.png" >]

[<img src="https://github.com/gopalritu/ZeroDowntime-OTA/blob/main/images/Project%20Structure/2.png" >]

[<img src="https://github.com/gopalritu/ZeroDowntime-OTA/blob/main/images/Project%20Structure/3.png" >]


## Main Files

- `ota_manager.cpp` – checks update and slots
- `boot_manager.cpp` – handles pending slot and boot attempts
- `health_monitor.cpp` – checks success or failure
- `watch_v1.cpp` – watch version 1
- `watch_v2.cpp` – watch version 2
- `watch_v3.cpp` – watch version 3 used for rollback testing
- `ota_status_driver.c` – Linux device driver
- `ota_status_ioctl.h` – ioctl definitions
- `scripts/` – setup and build scripts

---

## Linux Driver

The project has a Linux character device driver:

```text
/dev/ota_status
```

It stores:

- Active slot
- Pending slot
- Status
- Boot attempts

The C++ programs communicate with the driver using `ioctl()`.

To check the status:

```bash
sudo cat /dev/ota_status
```

Example:

```text
ACTIVE_SLOT=B PENDING_SLOT=NONE STATUS=SUCCESS BOOT_ATTEMPTS=0
```

---

## Watch Interface Testing

The OTA update process was tested using a simple **watch interface**.

Different watch applications were used to represent different software versions.

```text
watch_v1 → Version 1
watch_v2 → Version 2
watch_v3 → Version 3
```

This helped me visually test the update and rollback flow instead of testing only through the terminal.

---

## Run

Build the driver:

```bash
cd driver
make
sudo insmod ota_status_driver.ko
```

Build the OTA programs:

```bash
cd ../device/ota
g++ ota_manager.cpp -o ota_manager
g++ boot_manager.cpp -o boot_manager
g++ health_monitor.cpp -o health_monitor
```

Run:

```bash
./ota_manager
./boot_manager
./health_monitor success
```

For rollback testing:

```bash
./health_monitor fail
```


## Technologies

- C
- C++
- Linux
- Bash
- Linux Kernel Module
- Make
- Git / GitHub

---

## Limitations

This is a software prototype. The slots are represented using directories instead of real flash partitions and there is no physical embedded hardware.

---

## Future Scope

- Real embedded hardware
- Real A/B partitions
- Bootloader integration
- Remote OTA server
- Secure update verification

---

## Author

**Ritu Raj**

B.Tech CSE
