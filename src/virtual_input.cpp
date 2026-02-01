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
// VirtualWheel
// =======================
VirtualWheel::VirtualWheel() {}

VirtualWheel::~VirtualWheel() {
destroy();
}
    bool VirtualWheel::create() {
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) return false;

    // Enable events
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    // Enable scroll wheel
    ioctl(fd, UI_SET_RELBIT, REL_WHEEL);
    ioctl(fd, UI_SET_RELBIT, REL_WHEEL_HI_RES);  // For smoother scrolling

    struct uinput_setup usetup{};
    snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, "Huion H1161 Scroll Wheel");
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1;
    usetup.id.product = 0x2;
    usetup.id.version = 1;

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    sleep(1);
    return true;
}




void VirtualWheel::destroy() {
    if (fd >= 0) {
        ioctl(fd, UI_DEV_DESTROY);
        close(fd);
        fd = -1;
    }
}

    void VirtualWheel::handle_touch(bool active, int value) {
    if (fd < 0) return;

    // FIX 1: When finger lifted, DON'T process the jump to 0
    if (!active || value == 0) {
        tracking = false;
        last_value = 0;
        return;
    }

    // First contact - initialize, don't scroll
    if (!tracking) {
        tracking = true;
        last_value = value;
        return;
    }

    int delta = value - last_value;
    last_value = value;

    // FIX 2: Ignore huge jumps (errors/noise)
    if (abs(delta) > 300) {
        return;  // Don't scroll on big jumps
    }

    // Ignore tiny movements
    if (abs(delta) < 15) return;

    // Direction: sliding DOWN (value increases) = scroll DOWN
    int direction = (delta > 0) ? -1 : 1;

    // FIX 3: Use high-res wheel for smoother scrolling
    // High-res gives 120 units per "notch"
    int hi_res_amount = abs(delta) * 3;  // Adjust multiplier for sensitivity

    emit(fd, EV_REL, REL_WHEEL_HI_RES, direction * hi_res_amount);

    // FIX 4: Send standard wheel events less frequently for compatibility
    // Only send a "click" for larger movements
    if (abs(delta) >= 20) {
        int steps = abs(delta) / 20;
        if (steps > 3) steps = 3;  // Cap to prevent huge jumps

        for (int i = 0; i < steps; i++) {
            emit(fd, EV_REL, REL_WHEEL, direction);
        }
    }

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
