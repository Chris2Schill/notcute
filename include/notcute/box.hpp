#pragma once

#include <vector>
#include "rect.hpp"
#include "layout.hpp"

namespace notcute {

class Widget;

class BoxItem {
public:
    virtual ~BoxItem() = default;

    virtual Widget* get_widget() {
        return nullptr;
    }
};


class WidgetItem : public BoxItem {
public:
    WidgetItem(Widget* w) : wid(w) {}

    virtual Widget* get_widget() {
        return wid;
    }
private:
    Widget* wid;
};

class Box : public BoxItem {
public:
    Box(int rows, int cols, Box* parent = nullptr) : Box(parent) {
        set_size(rows, cols);
    }

    Box(Box* parent = nullptr) {
        ctx = notcute::get_layout_context();
        id = lay_item(ctx);

        if (parent) {
            lay_insert(ctx, parent->id, id);
        }
    }

    void set_size(int rows, int cols) {
        lay_set_size_xy(ctx, id, cols, rows);
    }

    void set_contain(int flags) {
        lay_set_contain(ctx, id, flags);
    }

    void set_behave(int flags) {
        lay_set_behave(ctx, id, flags);
    }

    void insert(Box* box) {
        lay_insert(ctx, id, box->id);
    }

    void run_context(){
        lay_run_context(ctx);
    }

    Widget* get_parent_widget() { return parent_widget; }
    void    set_parent_widget(Widget* pw) { parent_widget = pw; }

    const std::vector<BoxItem*>& get_children() const {
        return children;
    }

    void add_widget(Widget* widget);

    Rect get_rect() {
        lay_vec4 rect = lay_get_rect(ctx, id);
        return Rect {
            Point{rect[0],rect[1]},
            Size{(unsigned)rect[2],(unsigned)rect[3]},
        };
    }

    std::vector<BoxItem*> children;

    lay_id       id = {};
    lay_context* ctx = {};
    int          box_flags = 0;
    int          layout_flags = 0;
    Widget*      parent_widget = nullptr;
};

class VBoxLayout : public Box {
public:
    VBoxLayout(int rows, int cols, Box* parent = nullptr)
        : Box(rows, cols, parent)
    {
        set_behave(LAY_FILL | LAY_CENTER);
        set_contain(LAY_COLUMN);
    }

    VBoxLayout(Box* parent = nullptr)
        :Box(1,1,parent) 
    {
    }
};

class HBoxLayout : public Box {
public:
    HBoxLayout(int rows, int cols, Box* parent = nullptr)
        : Box(rows, cols, parent)
    {
        set_behave(LAY_FILL | LAY_CENTER);
        set_contain(LAY_ROW);
    }

    HBoxLayout(Box* parent = nullptr)
        :Box(1,1,parent) 
    {
    }
};

}
