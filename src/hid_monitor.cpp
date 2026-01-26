//
// Created by rohit on 1/26/26.
//

#include <libudev.h>
#include <poll.h>
#include <string>
#include <mutex>
#include <atomic>

#define HUION_VENDOR_ID "256c"
#define PEN_INTERFACE   "00"

void find_tab_hid(std::string& hid_path,
                  std::mutex& path_mutex,
                  std::atomic<bool>& path_updated)
{
    std::string active_sysname;

    struct udev* u = udev_new();
    if (!u) return;

    struct udev_monitor* mon =
        udev_monitor_new_from_netlink(u, "udev");

    udev_monitor_filter_add_match_subsystem_devtype(
        mon, "hidraw", nullptr);
    udev_monitor_enable_receiving(mon);

    const int fd = udev_monitor_get_fd(mon);

    while (true) {
        struct pollfd fds{};
        fds.fd = fd;
        fds.events = POLLIN;

        if (poll(&fds, 1, -1) <= 0)
            continue;

        struct udev_device* dev =
            udev_monitor_receive_device(mon);
        if (!dev)
            continue;

        const char* action  = udev_device_get_action(dev);
        const char* sysname = udev_device_get_sysname(dev);

        if (!action || !sysname) {
            udev_device_unref(dev);
            continue;
        }

        std::string act(action);

        if (act == "add") {
            // HID parent
            struct udev_device* hid_parent =
                udev_device_get_parent_with_subsystem_devtype(
                    dev, "hid", nullptr);
            if (!hid_parent) goto next;

            // USB interface
            struct udev_device* usb_iface =
                udev_device_get_parent_with_subsystem_devtype(
                    hid_parent, "usb", "usb_interface");
            if (!usb_iface) goto next;

            const char* iface =
                udev_device_get_sysattr_value(
                    usb_iface, "bInterfaceNumber");
            if (!iface || std::string(iface) != PEN_INTERFACE)
                goto next;

            // USB device (vendor check)
            struct udev_device* usb =
                udev_device_get_parent_with_subsystem_devtype(
                    dev, "usb", "usb_device");
            if (!usb) goto next;

            const char* vendor =
                udev_device_get_sysattr_value(usb, "idVendor");
            if (!vendor || std::string(vendor) != HUION_VENDOR_ID)
                goto next;

            const char* node =
                udev_device_get_devnode(dev);
            if (!node) goto next;

            {
                std::lock_guard<std::mutex> lock(path_mutex);
                hid_path = node;
                active_sysname = sysname;
                path_updated = true;
            }
        }
        else if (act == "remove") {
            if (active_sysname == sysname) {
                std::lock_guard<std::mutex> lock(path_mutex);
                hid_path.clear();
                active_sysname.clear();
                path_updated = true;
            }
        }

    next:
        udev_device_unref(dev);
    }

    udev_unref(u);
}

