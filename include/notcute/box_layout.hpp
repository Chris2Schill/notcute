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

        do_layout(rect);
    }

    void invalidate() override {
        set_dirty();
        // Layout::invalidate();

        set_geometry(get_geometry());
        // resize_children();
    }

    // void resize_children() {
    // }

    void add_layout(Layout* layout) override {
        set_dirty();
        items.push_back(layout);
        Layout::invalidate();
    }

    Point map_to_parent(Point pos) {
        if (Widget* p = dynamic_cast<Widget*>(get_parent_widget()); p) {
            Rect rect = p->get_geometry();
            pos.x -= rect.x();
            pos.y -= rect.y();
        }
        return pos;
    }

    void do_layout(const Rect& rect) {
        log_debug(debug_name + " do layout");
        int next_x = rect.x();
        int next_y = rect.y();

        // int w_height = rect.height()/ get_items().size();

        int left, top, right, bottom;
        get_content_margins(&left, &top, &right, &bottom);

        auto widget_height = (rect.height()-top-bottom) / items.size();


        // if (auto pw = get_parent_widget(); pw) {
        //     if (pw->get_name() == "row1") {
        //         log_debug(fmt::format("row1 container = {}", rect.to_string()));
        //     }
        // }
        // if (get_alignment() & Center) {
        //     next_x = next_x + rect.width()/2;
        // }

        

        for (auto item : get_items()) {
            int xoffset = 0;
            // if (get_alignment() & Center) {
            //     xoffset = -(item->get_geometry().width()/2);
            // }

            Widget* w = item->get_widget();

            Rect wrect = Rect{Point{next_x+xoffset, next_y}, item->size()};
            wrect.set_left(wrect.left() + left);
            wrect.set_top(wrect.top() + top);
            wrect.set_height(widget_height);
            wrect.set_width(rect.width()-left-right);

            log_debug(fmt::format("{} SIZER {}", w->get_name(), wrect.to_string()));

            wrect.m_pos = map_to_parent(wrect.pos());
            item->set_geometry(wrect);
            next_y += item->get_geometry().height();
        }
    }

private:
    LayoutDirection layout_direction = LEFT_TO_RIGHT;

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
