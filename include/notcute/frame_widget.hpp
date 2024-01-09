#pragma once

#include "widget.hpp"

namespace notcute {

class FrameWidget : public Widget {
public:
    using Widget::Widget;

    void set_layout(Box* box) override {
        Widget::set_layout(box);
    }

    void draw(ncpp::Plane* plane) override {
        draw_border(plane);
        // draw_coords(this);
    }

    void draw_border(ncpp::Plane* plane) {
        plane->perimeter_rounded(0,0,0);
    }
};

}
