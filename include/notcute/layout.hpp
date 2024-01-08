#pragma once

#include <vector>
#include <ncpp/NotCurses.hh>

#include "widget.hpp"
#include "object.hpp"
#include "logger.hpp"
#include "event_loop.hpp"

namespace notcute {

class Layout;
class LayoutItem;


enum Align {
    Left   = 1,
    Right  = 1 << 1,
    Top    = 1 << 2,
    Bottom = 1 << 3,
    VCenter = 1 << 4,
    HCenter = 1 << 5,
    Center = VCenter | HCenter,
};

class LayoutItem : public Object {
public:
    LayoutItem(Widget* parent = nullptr)
        : Object(parent) {

    }

    virtual Widget* get_widget() = 0;
    virtual Rect get_geometry() const = 0;
    virtual void set_geometry(const Rect&) = 0;
    virtual Layout* get_layout() { return nullptr; }

    virtual void invalidate() {};

    void set_alignment(int align) {
        alignment = align;
        set_geometry(get_geometry());
    }
    int get_alignment() const { return alignment; }

    Size size() const { return get_geometry().size(); }

protected:

    int alignment = 0;
};

class WidgetItem : public LayoutItem {
public:
    using LayoutItem::LayoutItem;

    WidgetItem(Widget* w) : widget(w) {}

    virtual Widget* get_widget() override {
        return widget;
    }

    virtual Rect get_geometry() const override {
        return widget->get_geometry();
    }

    void set_geometry(const Rect& rect) override {
        widget->set_geometry(rect);
    }

    Widget* widget = nullptr;
    friend class Layout;
};

class Layout : public LayoutItem {
public:
    std::string debug_name;
    Layout(Widget* parent = nullptr)
        : LayoutItem(parent)
    {
        if (parent) {
        }
        else {
            top_level = true;
        }
        EventLoop::get_instance()->subscribe<Event>(this, [this](Event* e){ widget_event(e); });
    }

    LayoutItem* item_at(int idx) {
        if (0 <= idx && idx < items.size()) {
            return items[idx];
        }
        return nullptr;
    }

    Layout* get_layout() override { return this; }

    virtual void add_layout(Layout* item) = 0;

    virtual void add_widget(Widget* widget) = 0;

    void get_content_margins(int* left, int* top, int* right, int* bottom) const {
        *left = 1;
        *top = 1;
        *right = 1;
        *bottom = 1;
    }

    Widget* get_widget() override { return nullptr; }

    // includes content geometry + margins
    Rect get_geometry() const override {
        return geometry;

        // Rect geo = {};
        // if (parent_widget) {
        //     geo = parent_widget->get_geometry();
        // }
        // int left, top, right, bottom;
        // get_content_margins(&left, &top, &right, &bottom);
        //
        // geo.m_size.width += left + right;
        // geo.m_size.height += top + bottom;
        // return geo;
    }

    void set_geometry(const Rect& rect) override {
        geometry = rect; 
    }

    Widget* get_parent_widget() const {
        return parent_widget;
    }
    void set_parent_widget(Widget* parent)  {
        parent_widget = parent;
        set_geometry(parent->get_geometry());
    }

    bool is_top_level() const { return top_level; }

    virtual void invalidate() override {
        // geometry = Rect{};
        // if (parent_widget) {
        //     set_geometry(parent_widget->get_geometry());
        // }
        update();
    }

    void widget_event(Event* e) {
        // if (e->get_sender() == parent_widget) {
            switch(e->get_type()) {
                case Event::LAYOUT_REQUEST:
                    activate();
                    break;
            }
        // }
    }

    void update() {
        Layout* layout = this;
        while(layout) {
            layout->activated = false;
            if (layout->is_top_level()) {
                Widget* w = layout->get_parent_widget();
                if (!w) {
                    throw std::runtime_error("Top level layouts parent widget is null");
                }
                log_debug("Sending layout request");
                EventLoop::get_instance()->post(new Event(w, Event::LAYOUT_REQUEST));
                break;
            }
            layout = static_cast<Layout*>(layout->parent);
        }
    }

    void set_dirty() {
        dirty = true;
    }

    virtual void do_resize() {
        log_debug("Geo " + debug_name + " do_resize()");

        Widget* mw = parent_widget;
        Rect rect;
        // if (mw) {
            rect = mw->get_contents_rect();
        // }
        // else {
        //     rect = get_geometry();
        // }

        set_geometry(rect);
    }

    void activate() {
        if (activated) {
            return;
        }

        activate_recursive_helper(this);
        do_resize();
    }

    void activate_recursive_helper(LayoutItem* item) {
        item->invalidate();
        if (Layout* self = item->get_layout(); self) {
            LayoutItem* child = nullptr;
            int i = 0;
            while ((child = self->item_at(i++))) {
                activate_recursive_helper(child);
            }
            self->activated = true;
        }
    }

    bool top_level = false;
    const std::vector<LayoutItem*>& get_items() { return items; }
protected:
    std::vector<LayoutItem*> items;

private:
    Widget* parent_widget = nullptr;
    Rect geometry = {};
    bool dirty = true;
    bool activated = false;
};


}
