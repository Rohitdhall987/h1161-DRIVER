//
// Created by rohit on 1/25/26.
//

#ifndef H1161_DRIVER_HID_READER_H
#define H1161_DRIVER_HID_READER_H

#include <string>
#include <vector>
#include <cstdint>

class HIDReader {
    std::string file_path;
    int fd = -1;

    public:
    HIDReader(std::string file);
    bool open();
    bool init();
    bool read(std::vector<uint8_t>&data) const;
    void close();
    virtual ~HIDReader();
};

#endif //H1161_DRIVER_HID_READER_H