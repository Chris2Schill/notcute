#pragma once

#include <boost/signals2.hpp>
#include <notcurses/notcurses.h>
#include "object.hpp"
#include "box.hpp"
#include "rect.hpp"

namespace notcute {


template<typename T>
using signal = boost::signals2::signal<T>;

class Layout;
class Event;
class DrawEvent;
class KeyboardEvent;
class ResizeEvent;

enum class FocusPolicy {
    FOCUS,
    NO_FOCUS
};

inline void fill(ncpp::Plane* plane, std::string c) {
    plane->erase();

    Rect rect {
        Point{
            .x = plane->get_x(),
            .y = plane->get_y(),
        },
        Size {
            .width = plane->get_dim_x(),
            .height = plane->get_dim_y(),
        }
    };

    for (int i = 0; i < rect.height(); i++) {
        for (int j = 0; j < rect.width(); j++) {
            plane->putstr(i, j, c.c_str());
        }
    }
}

class Widget : public Object {
public:
    Widget(Widget* parent = nullptr);
    ~Widget() {
        log_debug(fmt::format("deleting {} widget", get_name()));
        delete box;

        log_debug(fmt::format("deleting {}s ncplane", get_name()));
        delete plane;
    }

    virtual void set_layout(Box* layout);
    void    show();

    virtual void draw() {
        pre_draw(plane);
        draw(plane);
        post_draw(plane);
    }

    virtual void pre_draw(ncpp::Plane* plane) {
        plane->erase();
        box->run_context();

        int left = 0, top = 0, right = 0, bottom = 0;
        Rect rect = box->get_rect();

        // If we are nested inside of a child widget other than the top level widget
        // then we must translate the relative coords to global coords for positioning
        if (parent) {
            parent->get_layout()->get_margins_ltrb(&left, &top, &right, &bottom);
            parent->get_plane()->translate_abs(&rect.m_pos.y, &rect.m_pos.x);
        }

        plane->resize(rect.height(), rect.width());
        plane->move(rect.y(), rect.x());
        plane->resize_realign();
    }

    virtual void draw(ncpp::Plane* plane) { }

    virtual void post_draw(ncpp::Plane* plane) {
        draw_children();
    }

    void draw_children();

    void done_showing() {
        is_showing = false;
    }
    
    void reparent(Widget* new_parent) {
        parent = new_parent;
        if (new_parent) {
            plane->reparent_family(new_parent->plane);
        }
        else {
            plane->reparent_family((ncpp::Plane*)nullptr);
        }
        //
        // Box* layout = get_layout();
        // if (layout) {
        //     layout->parent
        // }
    }

    void set_name(const std::string& name) {
        plane_name_map[plane] = name;
    }

    const std::string& get_name() const {
        static const std::string NO_NAME = "(no_name)";
        const std::string& name = plane_name_map[plane];
        return name.empty() ? NO_NAME : name;
    }

    // void create_layout_item() {
    //     if (box->layout_item) {
    //         delete box->layout_item;
    //     }
    //
    //     if (parent) {
    //         box->layout_item = new Lay_Item(*parent->get_layout()->layout_item);
    //     }
    //     else {
    //         box->layout_item = new Lay_Item(box->rect.rows(), box->rect.cols());
    //         layout_ctx_map[this] = box->layout_item->get_context();
    //     }
    // }

    ncpp::Plane* get_plane() { return plane; }

    Box* get_layout() { return box; }

    void set_geometry(const Rect& rect);

    virtual bool on_event(Event* e);
    virtual bool on_draw_event(DrawEvent* e);
    virtual bool on_keyboard_event(KeyboardEvent* e);
    virtual bool on_resize_event(Event* e);

    static ncplane_options& default_options() {
        auto& nc = ncpp::NotCurses::get_instance();
        unsigned rows, cols;
        nc.get_term_dim(&rows, &cols);

        static ncplane_options opts = {
            .y = 0,
            .x = 0,
            .rows = 1,
            .cols = 1,
        };

        return opts;
    }

    Widget* get_parent() { return parent; }

    virtual std::string to_string() const {
        return "Widget " + get_name();
    }

    void set_focus() {
        if (focus_policy == FocusPolicy::FOCUS) {
            focused_widget = this;
        }
        else {
            log_debug(fmt::format("Tried to set focus on {} but focus policy is NO_FOCUS", get_name()));
        }
    }

    FocusPolicy get_focus_policy() const {
        return focus_policy;
    }

    void set_focus_policy(FocusPolicy fp) {
        focus_policy = fp;
    }

    static Widget*& get_focused_widget() {
        return focused_widget;
    }


    Rect get_geometry() {
        return get_layout()->get_rect();
    }

    bool is_top_level() {
        return get_parent() == nullptr;
    }

    Widget* get_top_level_widget() {
        Widget* w = this;
        while (!w->is_top_level()) {
            w = w->get_parent();
        }
        return w;
    }

    //
    // lay_context* Box::get_top_level_ctx(Widget* w) {
    //     auto iter = Widget::layout_ctx_map.find(w);
    //     if (iter == Widget::layout_ctx_map.end()) {
    //         return nullptr;
    //     }
    //     return iter->second;
    // }

protected:
    ncpp::Plane* plane = nullptr;
private:
    Box*         box = nullptr;
    Widget*      parent = nullptr;
    bool         is_showing = false; //not to be confused with a visible widget
    FocusPolicy  focus_policy = FocusPolicy::NO_FOCUS;
    static std::unordered_map<ncpp::Plane*, std::string> plane_name_map;
    static std::unordered_map<Widget*, lay_context*> layout_ctx_map;
    static Widget* focused_widget;
};

inline void draw_coords(Widget* w) {
    Rect rect = w->get_layout()->get_rect();
    w->get_plane()->putstr(0,0, fmt::format("({},{})", rect.y(), rect.x()).c_str());

    if (Widget* p = w->get_parent(); p) {
        p->get_plane()->translate_abs(&rect.m_pos.y, &rect.m_pos.x);
        std::string str = fmt::format("({},{})", rect.y(), rect.x());
        w->get_plane()->putstr(0,rect.width()-str.size(), str.c_str());
    }
}


}
