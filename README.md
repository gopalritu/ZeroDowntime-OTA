# ZeroDowntime-OTA

# ZeroDowntime OTA

### A/B Slot Based Rollback-Safe Embedded Linux OTA Update System

## About

This project is a simple software-based OTA update system for Embedded Linux.

The main idea is to use two slots, **A and B**, for software updates. One slot is active and the other slot is used for the new update.

If the new version works properly, it becomes active. If it fails, the system goes back to the previous working version.

I also implemented a Linux character device driver to show the OTA status using `/dev/ota_status`.

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
       Health Check
        /        \
       ↓          ↓
     PASS        FAIL
       |           |
       ↓           ↓
    SUCCESS     ROLLBACK
```

Example:

```text
Slot A → v1.0  (Active)
Slot B → v2.0  (Update)
```

If v2.0 works:

```text
Slot B → v2.0  (Active)
```

If the update fails, Slot A remains active.

---

## Project Structure

```text
ZeroDowntimeOTA/
├── device/
│   ├── app/
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

---

## Main Files

- `ota_manager.cpp` – checks update and slots
- `boot_manager.cpp` – handles pending slot and boot attempts
- `health_monitor.cpp` – checks success or failure
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

---

## Screenshots

<details>
<summary>Driver compilation</summary>

![Driver Compilation](screenshots/Driver%20compilation/1.png)

</details>

<details>
<summary>Initial system update</summary>

![Initial System Update](screenshots/Initial%20system%20update/1.png)

</details>

<details>
<summary>Interface</summary>

![Interface](screenshots/Interface/1.png)

</details>

<details>
<summary>Project Structure</summary>

![Project Structure 1](screenshots/Project%20Structure/1.png)

![Project Structure 2](screenshots/Project%20Structure/2.png)

![Project Structure 3](screenshots/Project%20Structure/3.png)

</details>

---

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
