//
// Created by rohit on 1/30/26.
//

#include "virtual_input.h"
#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

namespace tablet {

// =======================
// Helpers
// =======================
static void emit(int fd, int type, int code, int value) {
    input_event ev{};
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(fd, &ev, sizeof(ev));
}

// =======================
// VirtualPen
// =======================
VirtualPen::VirtualPen() {}
VirtualPen::~VirtualPen() { destroy(); }

bool VirtualPen::create() {
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) return false;

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_ABS);

    ioctl(fd, UI_SET_KEYBIT, BTN_TOOL_PEN);
    ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);
    ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS);
    ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS2);

    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE);

    uinput_user_dev dev{};
    snprintf(dev.name, UINPUT_MAX_NAME_SIZE, "Huion H1161 Pen");
    dev.id.bustype = BUS_USB;
    dev.id.vendor  = 0x256C;
    dev.id.product = 0x006D;
    dev.id.version = 1;

    dev.absmin[ABS_X] = 0;
    dev.absmax[ABS_X] = 65535;
    dev.absmin[ABS_Y] = 0;
    dev.absmax[ABS_Y] = 65535;
    dev.absmin[ABS_PRESSURE] = 0;
    dev.absmax[ABS_PRESSURE] = 8192;

    write(fd, &dev, sizeof(dev));
    ioctl(fd, UI_DEV_CREATE);

    return true;
}

void VirtualPen::destroy() {
    if (fd >= 0) {
        ioctl(fd, UI_DEV_DESTROY);
        close(fd);
        fd = -1;
    }
}

void VirtualPen::send_pen(bool in_range, bool tip,
                          bool btn1, bool btn2,
                          int x, int y, int pressure) {
    emit(fd, EV_KEY, BTN_TOOL_PEN, in_range);
    emit(fd, EV_KEY, BTN_TOUCH, tip);
    emit(fd, EV_KEY, BTN_STYLUS, btn1);
    emit(fd, EV_KEY, BTN_STYLUS2, btn2);

    emit(fd, EV_ABS, ABS_X, x);
    emit(fd, EV_ABS, ABS_Y, y);
    emit(fd, EV_ABS, ABS_PRESSURE, pressure);

    emit(fd, EV_SYN, SYN_REPORT, 0);
}

// =======================
// VirtualPad
// =======================
VirtualPad::VirtualPad() {}
VirtualPad::~VirtualPad() { destroy(); }

bool VirtualPad::create() {
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) return false;

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    for (int i = 0; i < 32; i++)
        ioctl(fd, UI_SET_KEYBIT, BTN_0 + i);

    for (int i = 0; i < 9; i++)
        ioctl(fd, UI_SET_KEYBIT, KEY_A + i);


    uinput_user_dev dev{};
    snprintf(dev.name, UINPUT_MAX_NAME_SIZE, "Huion H1161 Pad");
    dev.id.bustype = BUS_USB;
    dev.id.vendor  = 0x1;
    dev.id.product = 0x1;
    dev.id.version = 1;

    write(fd, &dev, sizeof(dev));
    ioctl(fd, UI_DEV_CREATE);

    return true;
}

void VirtualPad::destroy() {
    if (fd >= 0) {
        ioctl(fd, UI_DEV_DESTROY);
        close(fd);
        fd = -1;
    }
}

void VirtualPad::send_button(int index, bool pressed) const {
    if (index < 0 || index >= 32) return;
    emit(fd, EV_KEY, KEY_A + index, pressed);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}


}
