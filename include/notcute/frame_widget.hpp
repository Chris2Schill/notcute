#pragma once

#include "widget.hpp"

namespace notcute {

class FrameWidget : public Widget {
public:
    FrameWidget(int rows, int cols, Widget* parent = nullptr ) : Widget(parent) {}
    FrameWidget(Widget* parent = nullptr)
        : FrameWidget(
            parent ? parent->get_geometry().rows() : 1,
            parent ? parent->get_geometry().cols() : 1,
            parent) {
    }

    void set_title(const std::string& t) {
        title = t; 
    }

    void set_layout(Box* box) override {
        Widget::set_layout(box);
    }

    void draw(ncpp::Plane* plane) override {
        draw_border(plane);
        plane->putstr(0,2,title.c_str());
        // fill(plane, "f");
        // draw_coords(this);
    }

    void draw_border(ncpp::Plane* plane) {
        plane->perimeter_rounded(0,0,0);
    }

private:
    std::string title;

};

}
