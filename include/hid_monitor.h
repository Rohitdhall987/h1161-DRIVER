//
// Created by rohit on 1/26/26.
//

#ifndef H1161_DRIVER_HID_MONITOR_H
#define H1161_DRIVER_HID_MONITOR_H

#include <string>
void find_tab_hid(std::string& hid_path,
                  std::mutex& path_mutex,
                  std::atomic<bool>& path_updated);


#endif //H1161_DRIVER_HID_MONITOR_H