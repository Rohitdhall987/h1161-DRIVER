//
// Created by rohit on 1/24/26.
// ,,,,,,,,

#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <atomic>
#include <unistd.h>

#include "hid_monitor.h"
#include "hid_reader.h"
#include "utils.h"
#include "tablet_parser.h"
#include "virtual_input.h"

int main() {
    std::string device_hid_path;
    std::mutex path_mutex;
    std::atomic<bool> path_updated{false};

    std::thread monitor_thread([&]{
        find_tab_hid(device_hid_path, path_mutex, path_updated);
    });

    std::vector<uint8_t> data;
    data.reserve(64);

    HIDReader* reader = nullptr;
    std::string current_path;

    tablet::TabletParser parser;

    tablet::VirtualPad pad;
    pad.create();

    while (true) {
        // Check if device path changed
        if (path_updated.exchange(false)) {
            std::lock_guard lock(path_mutex);

            if (device_hid_path != current_path) {
                if (reader) {
                    delete reader;
                    reader = nullptr;
                }

                current_path = device_hid_path;


                if (!current_path.empty()) {
                    reader = new HIDReader(current_path);
                    if (!reader->open()) {
                        printerr("Failed to open USB device: " + current_path);
                        delete reader;
                        reader = nullptr;
                    } else {
                        print("Opened device: " + current_path);
                    }
                }
            }
        }

        if (reader) {
            data.clear();

            if (reader->read(data)) {
                //TODO: parse data and create virtual input
                tablet::TabletEvent ev = parser.parse(data);
                if (ev.type == tablet::ReportType::Pen) {
                    print("pen movement");
                } else if (ev.type == tablet::ReportType::SideButtons) {
                    static int last = -1;

                    if (ev.side.index != last) {
                        if (last != -1)
                            pad.send_button(last, false);

                        if (ev.side.index != -1)
                            pad.send_button(ev.side.index, true);

                        last = ev.side.index;
                    }


                } else if (ev.type == tablet::ReportType::TouchStrip) {
                    print("touch strip");
                }else if (ev.type == tablet::ReportType::Unknown) {
                    print("unknown event");
                }
            } else {
                printerr("Read failed, device may be disconnected");
                delete reader;
                reader = nullptr;
                current_path.clear();
                usleep(100000);
            }
        } else {
            usleep(100000);
        }
    }
}
