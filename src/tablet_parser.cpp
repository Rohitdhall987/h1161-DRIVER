//
// Created by rohit on 1/28/26.
//
#include "tablet_parser.h"

namespace tablet {

    static int first_set_bit(uint16_t v) {
        if (v == 0)
            return -1;

        for (int i = 0; i < 16; i++) {
            if (v & (1 << i))
                return i;
        }
        return -1;
    }

    TabletEvent TabletParser::parse(const std::vector<uint8_t>& data) {
        if (data.empty())
            return {};

        switch (data[0]) {
            case 0x08:
                return parsePen(data);
            case 0xF7:
                return parseSideButtons(data);
            case 0xF8:
                return parseTouchStrip(data);
            default:
                return {};
        }
    }

    // =====================
    // SIDE BUTTONS (0xF7)
    // =====================
    TabletEvent TabletParser::parseSideButtons(const std::vector<uint8_t>& d) {
        TabletEvent ev;
        ev.type = ReportType::SideButtons;

        if (d.size() < 12)
            return ev;

        // Assume button bits live in bytes 4 & 5
        uint16_t mask = d[4] | (d[5] << 8);

        ev.side.index = first_set_bit(mask & 0x03FF); // only 10 buttons

        return ev;
    }

    // =====================
    // PEN (0x08)
    // =====================
    TabletEvent TabletParser::parsePen(const std::vector<uint8_t>& d) {
        TabletEvent ev;
        ev.type = ReportType::Pen;

        if (d.size() < 12)
            return ev;

        auto& p = ev.pen;

        p.tip         = d[1] & 0x01;
        p.stylus_btn1 = d[1] & 0x02;
        p.stylus_btn2 = d[1] & 0x04;
        p.in_range    = d[1] & 0x80;

        p.x = d[2] | (d[3] << 8);
        p.y = d[4] | (d[5] << 8);

        p.pressure = d[6] | (d[7] << 8);

        p.pen_button_mask = d[8] | (d[9] << 8);

        return ev;
    }

    // =====================
    // TOUCH STRIP (0xF8)
    // =====================
    TabletEvent TabletParser::parseTouchStrip(const std::vector<uint8_t>& d) {
        TabletEvent ev;
        ev.type = ReportType::TouchStrip;

        if (d.size() < 12)
            return ev;

        auto& t = ev.touch;

        t.active = d[1] != 0;
        t.value = static_cast<int16_t>(d[4] | (d[5] << 8));

        return ev;
    }

} // namespace tablet
