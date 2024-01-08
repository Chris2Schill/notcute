#pragma once

#include "layout.hpp"
namespace notcute {

// class BoxLayout : public Layout {
// public:
//     using Layout::Layout;
// };
//
// class BoxLayoutItem : public LayoutItem {
// public:
//     using LayoutItem::LayoutItem;
//
// };

class BoxLayout : public Layout {
public:

    using Layout::Layout;

    enum LayoutDirection {
        LEFT_TO_RIGHT,
        TOP_TO_BOTTOM
    };



    void add_widget(Widget* widget) override {
        widget->reparent(get_parent_widget());
        items.push_back(new WidgetItem(widget));


        invalidate();
    }

    void set_geometry(const Rect& rect) override {
        log_debug(debug_name + " setting geometry");
        Layout::set_geometry(rect);

        if (items.size() == 0) {
            return;
        }

        int left, top, right, bottom;
        get_content_margins(&left, &top, &right, &bottom);

        auto widget_height = (rect.height()-top-bottom) / items.size();
        // Rect w_rect = widget->get_geometry();
        // w_rect.set_height(widget_height);
        // widget->set_geometry(rect);

        // for (auto item : get_items()) {
        //     Widget* w = item->get_widget();
        //     if (w) {
        //         Rect r = rect;
        //         // r.set_left(r.left() + left);
        //         // r.set_right(r.right() - right);
        //         // r.set_top(r.top() + top);
        //         // r.set_bottom(r.bottom() - bottom);
        //         r.m_pos.x += 1;
        //         // r.set_height(r.height() - top - bottom);
        //         // r.set_width(r.height() - top - bottom);
        //         r.set_height(widget_height);
        //
        //         w->set_geometry(r);
        //         // if (auto l = get_layout(); l) ,
        //         //     l->set_geometry(r);
        //         // }
        //     }
        // }


        do_layout(rect);
    }

    void invalidate() override {
        set_dirty();
        Layout::invalidate();
    }

    void add_layout(Layout* layout) override {
        set_dirty();
        items.push_back(layout);
        Layout::invalidate();
    }

    void do_layout(const Rect& rect) {
        log_debug(debug_name + " do layout");
        int next_x = rect.x();
        int next_y = rect.y();

        // int w_height = rect.height()/ get_items().size();

        int left, top, right, bottom;
        get_content_margins(&left, &top, &right, &bottom);

        auto widget_height = (rect.height()-top-bottom) / items.size();

        if (get_alignment() & Center) {
            next_x = next_x + rect.width()/2;
        }

        for (auto item : get_items()) {
            int xoffset = 0;
            if (get_alignment() & Center) {
                xoffset = -(item->get_geometry().width()/2);
            }

            Rect rect = Rect{Point{next_x+xoffset, next_y}, item->size()};
            rect.set_left(rect.left() + left);
            rect.set_top(rect.top() + top);
            rect.set_height(widget_height);
            rect.set_width(rect.width()-left-right);
            item->set_geometry(rect);
            next_y += item->get_geometry().height();
        }
    }

private:
    LayoutDirection layout_direction = TOP_TO_BOTTOM;

};

// class HBoxLayout : public BoxLayout {
// public:
//     using BoxLayout::BoxLayout;
//
//     void set_geometry(const Rect& rect) override {
//         BoxLayout::set_geometry(rect);
//         do_layout(rect);
//     }
//
//     void do_layout(const Rect& rect) {
//         log_debug(debug_name + " do layout");
//         int next_x = rect.x();
//         int next_y = rect.y();
//
//         if (get_alignment() & Center) {
//             next_y = next_y + rect.height()/2;
//         }
//
//         for (auto item : get_items()) {
//             int yoffset = 0;
//             if (get_alignment() & Center) {
//                 yoffset = -(item->get_geometry().height()/2);
//             }
//
//             item->set_geometry(Rect{Point{next_x, next_y+yoffset}, item->size()});
//             next_x += item->get_geometry().width();
//         }
//     }
// };
//

}
