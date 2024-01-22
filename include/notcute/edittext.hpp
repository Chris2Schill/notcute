#pragma once

#include <notcute/reader.hpp>

#include "widget.hpp"
#include "event_loop.hpp"

namespace notcute {

class EditText : public Widget {
public:
    EditText(Widget* parent = nullptr)
        : Widget(parent)
    {
        set_layout(new VBoxLayout(1, 25));
        set_focus_policy(FocusPolicy::FOCUS);
    }

    void draw(ncpp::Plane* plane) override {
        plane->putstr(buffer.c_str());
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch (e->get_key()) {
            case NCKEY_UP:
            case NCKEY_DOWN:
            case NCKEY_LEFT:
            case NCKEY_RIGHT:
                break;
            case NCKEY_ENTER:
                entry_submitted(buffer);
                redraw();
                return true;
            case NCKEY_BACKSPACE:
                if (!buffer.empty()) {
                    buffer.pop_back();
                }
                redraw();
                return true;
            default:
                if (buffer.size() < get_geometry().width()) {
                    buffer.push_back((char)e->get_key());
                }
                redraw();
                return true;
        }
        return false;
    }

    const std::string& get_contents() const { return buffer; }

    signal<void(const std::string&)> entry_submitted;

private:
    std::string buffer;
};

}
