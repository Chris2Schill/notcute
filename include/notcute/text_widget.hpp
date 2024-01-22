#pragma once

#include <string>
#include "widget.hpp"
#include "logger.hpp"
#include <algorithm>

namespace notcute {

class TextWidget : public Widget {
public:
    TextWidget(Widget* parent = nullptr): TextWidget("", parent) {}
    TextWidget(const std::string& text, Widget* parent = nullptr, uint32_t alignment = LAY_CENTER)
        : Widget(parent)
        ,align(alignment)
    {
        set_layout(new VBoxLayout);
        get_layout()->set_contain(LAY_ROW);
        get_layout()->set_behave(LAY_CENTER | LAY_HFILL);
        set_text(text);
    }

    void draw(ncpp::Plane* plane) override {

        int cols = plane->get_dim_x();

        int x = 0;
        if (align == LAY_LEFT) {
            x = 0;
        }
        else if (align == LAY_CENTER) {
            x = cols/2 - text.size()/2;
        }
        else if (align == LAY_RIGHT) {
            x = cols - text.size();
        }

        plane->set_channels(notcute::channels_from_fgbg(fg,bg));
        plane->putstr(0,x, text.c_str());
        log_debug(fmt::format("textwidget draw({}) plane_dim_yx({},{}) align={}, x={}",
                    text, plane->get_dim_y(), plane->get_dim_x(), align, x));
    }

    void set_scrolling(bool scroll) {
        get_plane()->set_scrolling(scroll);
    }

    // Separte from alignment of the widgets layout, this is for
    // aligning the text within the plane during the draw
    void set_align(uint32_t lay_align) {
        align = lay_align;
    }

    void set_text(const std::string& t) {
        text = t;
        int lines = std::ranges::count(t, '\n')+1;

        Rect r = get_geometry();
        r.set_width(text.size()/lines+1);
        r.set_height(lines);
        // r.set_width(text.size());
        // r.set_height(1);
        set_geometry(r);
        redraw();
        log_debug(fmt::format("textwidget set_text({}) resized({},{})", text, r.rows(), r.cols()));
    }

    std::string to_string() const override {
        return text;
    }

    void set_fg(Color c) { fg = c; }
    void set_bg(Color c) { bg = c; }

private:
    Color fg = {255,255,255,NCALPHA_OPAQUE};
    Color bg = {000,000,000,NCALPHA_TRANSPARENT};

    std::string text;
    uint32_t align = LAY_CENTER;
};

using Label = TextWidget;

}
