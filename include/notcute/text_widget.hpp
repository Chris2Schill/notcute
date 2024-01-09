#pragma once

#include <string>
#include "widget.hpp"
#include "logger.hpp"
#include <algorithm>

namespace notcute {

class TextWidget : public Widget {
public:
    TextWidget(const std::string& text, Widget* parent = nullptr)
        : Widget(parent)
    {
        set_layout(new VBoxLayout(parent));
        get_layout()->set_contain(LAY_LAYOUT);
        set_text(text);
    }

    void draw(ncpp::Plane* plane) override {
        plane->putstr(text.c_str());
    }

    void set_scrolling(bool scroll) {
        get_plane()->set_scrolling(scroll);
    }

    void set_text(const std::string& t) {
        text = t;
        int lines = std::ranges::count(t, '\n')+1;

        Rect r = {};
        r.set_width(text.size()/lines+1);
        r.set_height(lines);
        // r.set_width(text.size());
        // r.set_height(1);
        set_geometry(r);
    }

    std::string to_string() const override {
        return text;
    }

private:
    std::string text;
};

}
