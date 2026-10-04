#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../../driver/ota_status_ioctl.h"

using namespace std;


/*
 * Read the first line from a file.
 */
string readFile(const string& path)
{
    ifstream file(path);

    if (!file.is_open())
        return "";

    string value;
    getline(file, value);

    return value;
}


/*
 * Read state files from either:
 *
 * device/state/
 * or
 * device/ota/../state/
 */
string readStateFile(const string& filename)
{
    string value;

    value = readFile("state/" + filename);

    if (!value.empty())
        return value;

    return readFile("../state/" + filename);
}


/*
 * Write a value to a state file.
 */
bool writeStateFile(const string& filename, const string& value)
{
    string path = "state/" + filename;

    ofstream file(path);

    if (!file.is_open())
    {
        path = "../state/" + filename;
        file.open(path);
    }

    if (!file.is_open())
        return false;

    file << value << endl;

    return true;
}


/*
 * Send OTA state to Linux kernel driver.
 */
bool updateDriverStatus(char activeSlot,
                        char pendingSlot,
                        const string& status,
                        int bootAttempts)
{
    int fd = open("/dev/ota_status", O_RDWR);

    if (fd < 0)
    {
        perror("ERROR: Cannot open /dev/ota_status");
        return false;
    }

    ota_status_data data{};

    data.active_slot = activeSlot;
    data.pending_slot = pendingSlot;
    data.boot_attempts = bootAttempts;

    snprintf(data.status,
             sizeof(data.status),
             "%s",
             status.c_str());

    if (ioctl(fd, OTA_SET_STATUS, &data) < 0)
    {
        perror("ERROR: OTA_SET_STATUS failed");
        close(fd);
        return false;
    }

    close(fd);

    return true;
}


int main()
{
    cout << endl;
    cout << "=============================" << endl;
    cout << "         BOOT MANAGER" << endl;
    cout << "=============================" << endl;


    /*
     * Read current state.
     */
    string activeSlot =
        readStateFile("active_slot");

    string pendingSlot =
        readStateFile("pending_slot");

    string attemptsText =
        readStateFile("boot_attempts");


    if (activeSlot.empty())
    {
        cout << "ERROR: Active slot not found." << endl;
        return 1;
    }


    if (pendingSlot.empty())
    {
        pendingSlot = "NONE";
    }


    int bootAttempts = 0;

    if (!attemptsText.empty())
    {
        bootAttempts = stoi(attemptsText);
    }


    cout << "Current Active Slot : "
         << activeSlot << endl;

    cout << "Pending Slot        : "
         << pendingSlot << endl;

    cout << "Boot Attempts       : "
         << bootAttempts << endl;


    /*
     * No pending update.
     */
    if (pendingSlot == "NONE")
    {
        cout << endl;
        cout << "No pending update." << endl;

        updateDriverStatus(
            activeSlot[0],
            'N',
            "IDLE",
            bootAttempts
        );

        cout << "Linux Driver        : Updated" << endl;

        cout << "=============================" << endl;

        return 0;
    }


    /*
     * Validate pending slot.
     */
    if (pendingSlot != "A" &&
        pendingSlot != "B")
    {
        cout << "ERROR: Invalid pending slot." << endl;
        return 1;
    }


    /*
     * Increase boot attempt count.
     */
    bootAttempts++;


    writeStateFile(
        "boot_attempts",
        to_string(bootAttempts)
    );


    /*
     * Mark system as testing.
     */
    writeStateFile(
        "update_status",
        "TESTING"
    );


    cout << endl;
    cout << "Booting pending slot..." << endl;

    cout << "Boot Slot            : "
         << pendingSlot << endl;

    cout << "Boot Attempt         : "
         << bootAttempts << endl;

    cout << "Status               : TESTING" << endl;


    /*
     * Inform Linux kernel driver.
     */
    updateDriverStatus(
        activeSlot[0],
        pendingSlot[0],
        "TESTING",
        bootAttempts
    );


    cout << "Linux Driver         : Status updated" << endl;

    cout << "=============================" << endl;

    return 0;
}
