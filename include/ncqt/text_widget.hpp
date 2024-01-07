#pragma once

#include <string>
#include "widget.hpp"
#include "logger.hpp"

namespace ncqt {

class TextWidget : public Widget {
public:
    TextWidget(const std::string& text, Widget* parent = nullptr)
        : Widget(parent)
    {
        set_text(text);
    }

    void draw(ncpp::Plane* plane) override {
        plane->erase();
        plane->putstr(text.c_str());
    }

    void set_text(const std::string& t) {
        text = t;

        auto geo = get_geometry();
        geo.m_size.width = text.size();
        geo.m_size.height = 1;
        set_geometry(geo);
    }

private:
    std::string text;
};

}
