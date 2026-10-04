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
 * Read state files.
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
 * Write state files.
 */
bool writeStateFile(const string& filename,
                    const string& value)
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
 * Update Linux kernel driver.
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


int main(int argc, char* argv[])
{
    cout << endl;
    cout << "=============================" << endl;
    cout << "       HEALTH MONITOR" << endl;
    cout << "=============================" << endl;


    /*
     * Health result must be supplied.
     *
     * success -> commit update
     * fail    -> rollback
     */
    if (argc != 2)
    {
        cout << "Usage:" << endl;
        cout << "  sudo ./health_monitor success" << endl;
        cout << "  sudo ./health_monitor fail" << endl;

        return 1;
    }


    string result = argv[1];


    if (result != "success" &&
        result != "fail")
    {
        cout << "ERROR: Invalid health result." << endl;

        cout << "Use:" << endl;
        cout << "  success" << endl;
        cout << "  fail" << endl;

        return 1;
    }


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


    if (pendingSlot.empty() ||
        pendingSlot == "NONE")
    {
        cout << "ERROR: No pending slot available." << endl;
        return 1;
    }


    int bootAttempts = 0;

    if (!attemptsText.empty())
    {
        bootAttempts = stoi(attemptsText);
    }


    cout << "Active Slot  : "
         << activeSlot << endl;

    cout << "Pending Slot : "
         << pendingSlot << endl;

    cout << "Health Result : "
         << result << endl;


    /*
     * SUCCESS
     *
     * New slot becomes active.
     */
    if (result == "success")
    {
        cout << endl;
        cout << "Health check PASSED." << endl;
        cout << "Committing new slot..." << endl;


        writeStateFile(
            "active_slot",
            pendingSlot
        );


        writeStateFile(
            "pending_slot",
            "NONE"
        );


        writeStateFile(
            "boot_attempts",
            "0"
        );


        writeStateFile(
            "update_status",
            "SUCCESS"
        );


        /*
         * Driver now reports the new active slot.
         */
        updateDriverStatus(
            pendingSlot[0],
            'N',
            "SUCCESS",
            0
        );


        cout << "New Active Slot : "
             << pendingSlot << endl;

        cout << "Status          : SUCCESS" << endl;

        cout << "Linux Driver    : Status updated" << endl;
    }


    /*
     * FAIL
     *
     * Keep old active slot.
     * Clear pending slot.
     */
    else
    {
        cout << endl;
        cout << "Health check FAILED." << endl;
        cout << "Rolling back..." << endl;


        writeStateFile(
            "pending_slot",
            "NONE"
        );


        writeStateFile(
            "boot_attempts",
            "0"
        );


        writeStateFile(
            "update_status",
            "ROLLBACK"
        );


        /*
         * Old active slot remains active.
         */
        updateDriverStatus(
            activeSlot[0],
            'N',
            "ROLLBACK",
            0
        );


        cout << "Active Slot    : "
             << activeSlot << endl;

        cout << "Rollback       : COMPLETED" << endl;

        cout << "Linux Driver   : Status updated" << endl;
    }


    cout << "=============================" << endl;

    return 0;
}
