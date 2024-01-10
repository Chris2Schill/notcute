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
            case NCKEY_ESC:
                break;
            case NCKEY_ENTER:
                entry_submitted(buffer);
                return true;
            case NCKEY_BACKSPACE:
                if (!buffer.empty()) {
                    buffer.pop_back();
                }
                return true;
            default:
                if (buffer.size() < get_geometry().width()) {
                    buffer.push_back((char)e->get_key());
                }
                return true;
        }
        return false;
    }

    const std::string& get_contents() const { return buffer; }

    signal<void(const std::string&)> entry_submitted;

private:
    std::string buffer;
};

// class EditText2 : public Widget {
// public:
//     EditText2(Widget* parent = nullptr)
//         : Widget(parent)
//     {
//         set_layout(new VBoxLayout(1, 25, parent));
//         sub_plane = new ncpp::Plane(*get_plane());
//         sub_plane->reparent(plane);
//         sub_plane->resize(1, 24);
//         sub_plane->resize_realign();
//
//         reader_box = new HBoxLayout(1,25, this);
//         reader_box->set_margins_ltrb(1,1,1,1);
//         get_layout()->insert(reader_box);
//
//         ncreader_options opts {
//             .tchannels = NCCHANNEL_INITIALIZER(255,255,255),
//             .flags = NCREADER_OPTION_HORSCROLL | NCREADER_OPTION_CURSOR | NCREADER_OPTION_NOCMDKEYS
//         };
//         reader = new notcute::Reader(sub_plane, &opts);
//
//         set_focus_policy(FocusPolicy::FOCUS);
//     }
//
//     void draw() override {
//         Widget::pre_draw(plane);
//         // Widget::pre_draw(reader->get_plane());
//
//         Widget::draw(plane);
//         // Widget::draw(reader->get_plane());
//
//         Widget::post_draw(plane);
//         // Widget::post_draw(reader->get_plane());
//     }
//
//     // void pre_draw(ncpp::Plane* p) override {
//     //     Widget::pre_draw(reader->get_plane());
//     //     // Widget::pre_draw(sub_plane);
//     //     // Rect rect = reader_box->get_rect();
//     //     // reader->get_plane()->move(rect.y(), rect.x());
//     // }
//
//     // void draw(ncpp::Plane* plane) override {
//     //     // fill(plane, "z");
//     // }
//     // void post_draw(ncpp::Plane* p) override {}
//
//     void on_keyboard_event(KeyboardEvent* e) override {
//         switch (e->get_key()) {
//             case NCKEY_ENTER:
//                 entry_submitted(reader->get_contents());
//                 return;
//         }
//
//         log_debug("sending input to reader");
//         ncinput ni = e->get_ncinput();
//         reader->offer_input(&ni);
//
//         // log_debug(fmt::format("keyboard event {}", e->get_key()));
//     }
//
//     boost::signals2::signal<void(const char*)> entry_submitted;
//
// private:
//     Reader* reader = nullptr;
//     ncpp::Plane*  sub_plane = nullptr;
//     Box* reader_box = nullptr;
// };


}
