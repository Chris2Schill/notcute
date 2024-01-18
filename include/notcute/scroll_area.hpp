#pragma once

#include <notcurses/notcurses.h>
#include <ncpp/Visual.hh>
#include <string>

#include "frame_widget.hpp"
#include "event_loop.hpp"
#include "glyphs.hpp"

namespace notcute {

class ScrollArea : public FrameWidget {
public:
    ScrollArea(Widget* content, Widget* parent = nullptr)
        :FrameWidget(parent)
        ,content(content)
    {
        set_layout(new VBoxLayout(20,20));
        // get_layout()->add_widget(content);
        if (content) {
            set_content(content);
        }
        get_plane()->set_bg_alpha(NCALPHA_OPAQUE);
        set_focus_policy(FocusPolicy::FOCUS);
    }
    ~ScrollArea() {
    }

    void draw(ncpp::Plane* p) override;
    void draw2(ncpp::Plane* p);
    void draw3(ncpp::Plane* p);

    ncpp::Plane* create_flat_merged_plane(Widget* p);
    ncpp::Plane* create_flat_merged_plane_recursive_helper(Widget* p);
    void draw_children(Widget* w);


    void set_content(Widget* w) { 
        content = w;

        content->get_layout()->set_margins_ltrb(1,1,1,1);
        // content->reparent(nullptr);
        // // get_layout()->add_widget(content);
        // get_layout()->add_widget(content);
        Rect r = get_geometry();
        content->get_plane()->move(r.y()+1, r.x()+1);
        content->get_layout()->set_behave(LAY_FILL);
        // content->get_plane()->move_bottom();
        // Rect rect = get_geometry();
        // content->get_layout()->set_size(rect.rows(), rect.cols());
    }
    
    bool on_keyboard_event(KeyboardEvent* e) override {
        switch (e->get_key()) {
            case 'h':
            case NCKEY_LEFT:
                content_subwindow.x -= 1;
                redraw();
                break;
            case 'l':
            case NCKEY_RIGHT:
                content_subwindow.x += 1;
                redraw();
                break;
            case 'j':
            case NCKEY_DOWN:
                content_subwindow.y += 1;
                redraw();
                return true;
            case 'k':
            case NCKEY_UP:
                content_subwindow.y -= 1;
                redraw();
                return true;
            default:
                return false;
        }
        notcute::log_debug(fmt::format("content_subwindow_yx=({},{})", content_subwindow.y, content_subwindow.x));

        return false;
    }

    // void draw(ncpp::Plane* plane) override {
    //     // int i = 0;
    //     // for (int row = row_start;
    //     //      // row < items.size() && i <= get_layout()->get_rect().height();
    //     //      i <= get_layout()->get_rect().height();
    //     //      ++i, ++row)
    //     // {
    //     //     // ListItem*& item = items[row];
    //     // }
    // }

    // Signals
    boost::signals2::signal<void(int)> scrolled;

private:
    struct {
        int y = 0;
        int x = 0;
    } content_subwindow;;

    int row_start = 0;
    Widget* content;
};

}
