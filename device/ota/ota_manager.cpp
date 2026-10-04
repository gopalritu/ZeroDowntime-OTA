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
    {
        return "";
    }

    string value;
    getline(file, value);

    return value;
}


/*
 * Find the state file.

 * This allows the program to work when executed from:
 *   ~/ZeroDowntimeOTA/device
 * or
 *   ~/ZeroDowntimeOTA/device/ota
 */
string readStateFile(const string& filename)
{
    string value;

    value = readFile("state/" + filename);

    if (!value.empty())
    {
        return value;
    }

    value = readFile("../state/" + filename);

    return value;
}


/*
 * Send current OTA state to the Linux kernel driver.
 */
bool updateDriverStatus(char activeSlot,
                        char pendingSlot,
                        const string& status,
                        int bootAttempts)
{
    int fd = open("/dev/ota_status", O_RDWR);

    if (fd < 0)
    {
        perror("ERROR: Could not open /dev/ota_status");
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
    /*
     * Read the currently active slot.
     */
    string activeSlot = readStateFile("active_slot");

    if (activeSlot.empty())
    {
        cout << "ERROR: Could not determine active slot." << endl;
        return 1;
    }


    /*
     * Determine the inactive slot.
     */
    string inactiveSlot;

    if (activeSlot == "A")
    {
        inactiveSlot = "B";
    }
    else if (activeSlot == "B")
    {
        inactiveSlot = "A";
    }
    else
    {
        cout << "ERROR: Invalid active slot." << endl;
        return 1;
    }


    /*
     * Convert slot names to directory names.
     */
    string activeDir;
    string inactiveDir;

    if (activeSlot == "A")
    {
        activeDir = "../slot_a";
        inactiveDir = "../slot_b";
    }
    else
    {
        activeDir = "../slot_b";
        inactiveDir = "../slot_a";
    }


    /*
     * Read versions.
     */
    string activeVersion =
        readFile(activeDir + "/version.txt");

    string inactiveVersion =
        readFile(inactiveDir + "/version.txt");


    if (activeVersion.empty())
    {
        cout << "ERROR: Active slot version not found." << endl;
        return 1;
    }

    if (inactiveVersion.empty())
    {
        cout << "ERROR: Inactive slot version not found." << endl;
        return 1;
    }


    /*
     * Read boot attempts.
     */
    string bootAttemptsText =
        readStateFile("boot_attempts");

    int bootAttempts = 0;

    if (!bootAttemptsText.empty())
    {
        bootAttempts = stoi(bootAttemptsText);
    }


    /*
     * Display OTA information.
     */
    cout << endl;
    cout << "=============================" << endl;
    cout << "         OTA MANAGER" << endl;
    cout << "=============================" << endl;

    cout << "Active Slot   : " << activeSlot << endl;
    cout << "Active Version: " << activeVersion << endl;

    cout << "Inactive Slot : " << inactiveSlot << endl;
    cout << "Inactive Ver. : " << inactiveVersion << endl;

    cout << "=============================" << endl;


    /*
     * Check whether an update is available.
     */
    string status;

    if (activeVersion != inactiveVersion)
    {
        cout << "Update available." << endl;
        status = "UPDATE_AVAILABLE";
    }
    else
    {
        cout << "System is up to date." << endl;
        status = "IDLE";
    }

    cout << "=============================" << endl;


    /*
     * Send OTA information to Linux kernel driver.
     */
    char activeSlotChar = activeSlot[0];
    char inactiveSlotChar = inactiveSlot[0];

    if (updateDriverStatus(
            activeSlotChar,
            inactiveSlotChar,
            status,
            bootAttempts))
    {
        cout << "Linux Driver  : Status updated successfully." << endl;
    }
    else
    {
        cout << "Linux Driver  : Status update failed." << endl;
    }

    cout << "=============================" << endl;

    return 0;
}
