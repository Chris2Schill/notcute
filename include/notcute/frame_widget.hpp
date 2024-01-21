#pragma once

#include "widget.hpp"
#include <notcurses/notcurses.h>

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

    void draw(ncpp::Plane* plane) override {
        draw_border(plane);
        draw_title(plane);
        Widget::draw(plane);
    }

    virtual void draw_border(ncpp::Plane* plane) {
        uint64_t fg = frame_fg.to_channel();
        uint64_t bg = frame_bg.to_channel();
        // bg &= NCALPHA_TRANSPARENT;
        uint64_t channels = (fg<<32) + bg;
        plane->perimeter_rounded(0, channels, 0);
    }

    void draw_title(ncpp::Plane* plane) {
        auto chans = plane->get_channels();

        plane->set_channels(channels_from_fgbg(frame_title_fg, frame_title_bg));
        plane->putstr(0,2,title.c_str());

        plane->set_channels(chans);
    }

    void set_frame_fg(Color c) { frame_fg = c; }
    void set_frame_bg(Color c) { frame_bg = c; }
    void set_frame_title_fg(Color c) { frame_title_fg = c; }
    void set_frame_title_bg(Color c) { frame_title_bg = c; }

    Color get_frame_fg() const { return frame_fg; }
    Color get_frame_bg() const { return frame_bg; }
    Color get_frame_title_fg() const { return frame_title_fg; }
    Color get_frame_title_bg() const { return frame_title_bg; }

    struct Defaults {
        Color       frame_fg       = { 255,255,255, NCALPHA_OPAQUE};
        Color       frame_bg       = { 000,000,000, NCALPHA_TRANSPARENT};
        Color       frame_title_fg = { 255,255,255, NCALPHA_OPAQUE};
        Color       frame_title_bg = { 000,000,000, NCALPHA_TRANSPARENT};
    };
    static Defaults defaults;

private:
    std::string title;
    Color       frame_fg       = defaults.frame_fg;
    Color       frame_bg       = defaults.frame_bg;
    Color       frame_title_fg = defaults.frame_title_fg;
    Color       frame_title_bg = defaults.frame_title_bg;

};

}
