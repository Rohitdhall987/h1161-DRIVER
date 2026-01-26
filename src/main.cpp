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

    while (true) {
        // Check if device path changed
        if (path_updated.exchange(false)) {
            std::lock_guard<std::mutex> lock(path_mutex);

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

    if (reader) delete reader;
    monitor_thread.join();

    return 0;
}
