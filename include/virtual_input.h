//
// Created by rohit on 1/30/26.
//

#ifndef ARCH_H1161_DRIVER_VIRTUAL_INPUT_H
#define ARCH_H1161_DRIVER_VIRTUAL_INPUT_H

#include <linux/input.h>
#include <string>

namespace tablet {

    class VirtualPen {
    public:
        VirtualPen();
        ~VirtualPen();

        bool create();
        void destroy();

        void send_pen(bool in_range, bool tip,
                      bool btn1, bool btn2,
                      int x, int y, int pressure);

    private:
        int fd = -1;
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