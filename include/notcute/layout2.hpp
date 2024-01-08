#pragma once

#include <notcute/layout2.h>
#include <notcute/rect.hpp>
#include <notcute/widget.hpp>

namespace notcute {
class Layout;


lay_context* get_layout_context();

class LayoutItem {

public:
    virtual Widget* get_widget() {
        return nullptr;
    }

    virtual Layout* get_layout() { return nullptr; }
};

class WidgetItem : public LayoutItem {
public:
    WidgetItem(Widget* w) : wid(w) {}

    virtual Widget* get_widget() {
        return wid;
    }

    Widget* wid;
};

class Layout : public LayoutItem {
public:

    Widget* get_widget() override {
        return parent_widget;
    }

    virtual void set_geometry(const Rect& r) {
        rect = r;
    }

    Rect get_geometry() const { return rect; }

    Layout* get_layout() override { return this; }


    virtual void add_widget(Widget*) = 0;

    void set_parent_widget(notcute::Widget* pw) { parent_widget = pw; }

    virtual const std::vector<LayoutItem*>& get_children() const = 0;
    Rect rect;
    notcute::Widget* parent_widget = nullptr;
};


class BoxLayout : public Layout {
public:

    BoxLayout() {
        id = lay_item(get_layout_context());

    }

    void add_widget(Widget* w) override {
        children.push_back(new WidgetItem(w));
        // lay_insert(get_layout_context(), id, w->get_lay_id());
    }

    void set_geometry(const Rect& r) override {
        Layout::set_geometry(r);
    }

    const std::vector<LayoutItem*>& get_children() const override { return children; }
    std::vector<LayoutItem*> children;
    

    Rect get_geometry() const { return rect; } 


    lay_box_flags layout_direction = LAY_COLUMN;
    lay_id id;
    // Create our root item. Items are just 2D boxes.
};

}
