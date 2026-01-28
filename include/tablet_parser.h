//
// Created by rohit on 1/28/26.
//

#ifndef H1161_DRIVER_TABLET_PARSER_H
#define H1161_DRIVER_TABLET_PARSER_H

#include <cstdint>
#include <vector>

namespace tablet {

    enum class ReportType {
        Unknown,
        Pen,
        SideButtons,
        TouchStrip
    };

    struct SideButtonEvent {
        int index = -1;
    };

    struct PenState {
        bool in_range = false;
        bool tip = false;
        bool stylus_btn1 = false;
        bool stylus_btn2 = false;

        uint16_t x = 0;
        uint16_t y = 0;
        uint16_t pressure = 0;

        // 16 pen-only buttons (still stateful)
        uint16_t pen_button_mask = 0;
    };

    struct TouchStripState {
        bool active = false;
        int16_t value = 0;
    };

    struct TabletEvent {
        ReportType type = ReportType::Unknown;
        PenState pen;
        SideButtonEvent side;
        TouchStripState touch;
    };

    class TabletParser {
    public:
        TabletEvent parse(const std::vector<uint8_t>& data);

    private:
        TabletEvent parsePen(const std::vector<uint8_t>& data);
        TabletEvent parseSideButtons(const std::vector<uint8_t>& data);
        TabletEvent parseTouchStrip(const std::vector<uint8_t>& data);
    };

} // namespace tablet



#endif //H1161_DRIVER_TABLET_PARSER_H