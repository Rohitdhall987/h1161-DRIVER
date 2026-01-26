//
// Created by rohit on 1/25/26.
//

#include "hid_reader.h"

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <utility>
#include <poll.h>

HIDReader::HIDReader(std::string  file)
    : file_path(std::move(file)) {}

bool HIDReader::open() {
    if (file_path.empty())
        return false;

    fd = ::open(file_path.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);

    if (fd < 0) {
        perror("hidraw open");
        return false;
    }

    return true;
}

bool HIDReader::init() {
    if (fd < 0)
        return false;

    uint8_t init1[] = { 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    ssize_t w = ::write(fd, init1, sizeof(init1));

    if (w < 0) { perror("hidraw init write");
        return false;
    }
    return true;
}

bool HIDReader::read(std::vector<uint8_t>& data) const {
    struct pollfd pfd{};
    pfd.fd = fd;
    pfd.events = POLLIN;

    int ret = poll(&pfd, 1, -1); // block until event
    if (ret < 0) {
        perror("poll");
        return false;
    }

    if (!(pfd.revents & POLLIN))
        return false;

    uint8_t buf[64];
    ssize_t n = ::read(fd, buf, sizeof(buf));
    if (n < 0) {
        if (errno == EAGAIN)
            return false;
        perror("read");
        return false;
    }

    data.assign(buf, buf + n);
    return true;
}



void HIDReader::close() {
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

HIDReader::~HIDReader() {
    close();
}
