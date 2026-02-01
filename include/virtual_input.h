//
// Created by rohit on 1/30/26.
//

#ifndef ARCH_H1161_DRIVER_VIRTUAL_INPUT_H
#define ARCH_H1161_DRIVER_VIRTUAL_INPUT_H

#include <linux/input.h>
#include <string>

namespace tablet {

    class VirtualWheel {
        int fd = -1;
        int last_value = 0;
        bool tracking = false;

    public:
        VirtualWheel();
        ~VirtualWheel();

        bool create();
        void destroy();
        void handle_touch(bool active, int value);
    };


    class VirtualPad {
    public:
        VirtualPad();
        ~VirtualPad();

        bool create();
        void destroy();

        void send_button(int index, bool pressed) const;

    private:
        int fd = -1;
    };

}


#endif //ARCH_H1161_DRIVER_VIRTUAL_INPUT_H