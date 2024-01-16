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

    // Use this to use the frame as a decorator for any widget.
    static FrameWidget* wrap(Widget* wrappee) {
        auto frame = new FrameWidget();
        auto layout = new VBoxLayout(frame);
        wrappee->get_layout()->set_margins_ltrb(1,1,1,1);
        layout->add_widget(wrappee);

        Rect rect = wrappee->get_geometry();
        layout->set_size(rect.rows()+2, rect.cols()+2);
        return frame;
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
        Widget::draw(plane);
        // ncpp::Cell c('g');
        // plane->polyfill(1,1,c);
    }

    void draw_border(ncpp::Plane* plane) {
        uint64_t fg, bg;
        fg = NCCHANNEL_INITIALIZER(255,0,0);
        bg = NCCHANNEL_INITIALIZER(0,0,0);
        bg &= NCALPHA_TRANSPARENT;
        uint64_t channels = (fg<<32) + bg;
        plane->perimeter_rounded(0, channels, 0);
        // ncpp::Cell c('+');
        // plane->perimeter(c,c,c,c,c,c, 0);
    }

private:
    std::string title;

};

}
