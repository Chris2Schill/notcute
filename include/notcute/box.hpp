#pragma once

#include <vector>
#include <sstream>
#include "layout.h"
#include "rect.hpp"
#include "layout.hpp"

namespace notcute {

class Widget;
class Box;

class BoxItem {
public:
    virtual ~BoxItem() = default;

    virtual Widget* get_widget() {
        return nullptr;
    }

    virtual Box* get_layout() { return nullptr; }

    virtual void release_ownership() {}
};


class WidgetItem : public BoxItem {
public:
    WidgetItem(Widget* w) : wid(w) {}
    ~WidgetItem();

    virtual Widget* get_widget() override {
        return wid;
    }

    void release_ownership() override;

    Box* get_layout() override ;

private:
    Widget* wid;
};

class Lay_Item {
public:
    Lay_Item(int rows, int cols) {
        ctx =  new lay_context;
        lay_init_context(ctx);
        lay_reserve_items_capacity(ctx, 1024);
        id = lay_item(ctx);
        set_size(rows, cols);
        log_debug(fmt::format("LAY_ITEM created top level lay_item: id = {}", id));
    }
    Lay_Item(Lay_Item& parent) {
        ctx = parent.ctx;
        id = lay_item(ctx);
        log_debug(fmt::format("LAY_ITEM created sub level lay_item: id = {}", id));
    }

    ~Lay_Item() {
        // 0 id means its the root of the view tree
        log_debug(fmt::format("LAY_ITEM deleting lay_item: id = {}", id));
        if (id == 0) {
            log_debug(fmt::format("LAY_ITEM destroyed context: id = {}", id));
            lay_destroy_context(ctx);
            delete ctx;
        }
    }

    // Wrapper functions to the c interface
    void set_size(int rows, int cols) { lay_set_size_xy(ctx, id, cols, rows); }
    void set_contain(int flags) { lay_set_contain(ctx, id, flags); }
    void set_behave(int flags) { lay_set_behave(ctx, id, flags); }
    void insert(Lay_Item* item)
    {
        lay_insert(ctx, id, item->id);
    }
    void set_margins_ltrb(lay_scalar left, lay_scalar top, lay_scalar right, lay_scalar bottom) { lay_set_margins_ltrb(ctx,id,left, top, right, bottom); }
    void get_margins_ltrb(lay_scalar* left, lay_scalar* top, lay_scalar* right, lay_scalar* bottom) { lay_get_margins_ltrb(ctx,id,left,top,right, bottom); }
    void run_context() { lay_run_context(ctx); }
    void reset_context() { lay_reset_context(ctx); }
    void run_item() { lay_run_item(ctx, id); }
    lay_context* get_context() { return ctx; }
    lay_vec4 get_rect() { return lay_get_rect(ctx, id); }
    std::function<void(Lay_Item*)> resize_callback;

private:
    lay_id       id = {};
    lay_context* ctx = nullptr;
};

class Box : public BoxItem {
public:

    Box(int rows, int cols, Widget* parent = nullptr);
    Box(Widget* parent = nullptr) : Box(1,1, parent) {}
    ~Box();

    void set_size(int rows, int cols) {
        rect.set_height(rows);
        rect.set_width(cols);

        if (layout_item) {
            layout_item->set_size(rows, cols);
        }
    }

    Widget* get_widget() override { return get_parent_widget(); }

    void set_contain(int flags) {
        contain_flags = flags;
        if (layout_item) {
            layout_item->set_contain(flags);
        }
        else {
            log_debug("lay_item failed to set contain: layout_item is null");
        }
    }

    void set_behave(int flags) {
        behave_flags = flags;
        if (layout_item) {
            layout_item->set_behave(flags);
        }
        else {
            log_debug("lay_item failed to set contain: layout_item is null");
        }
    }

    void add_widgets(std::vector<notcute::Widget*> ws) {
        for (auto& w : ws) {
            add_widget(w);
        }
    }

    // void insert(Box* box) {
    //     if (layout_item) {
    //         layout_item->insert(box->layout_item);
    //     }
    //     else {
    //         log_debug("lay_item failed to insert: layout_item is null");
    //     }
    //     
    //     // lay_item_t *pchild = lay_get_item(ctx, id);
    //     // LAY_ASSERT(!(pchild->flags & LAY_ITEM_INSERTED));
    //     // id = lay_item(ctx);
    //     // lay_insert(ctx, id, box->id);
    // }

    // lay_context* get_top_level_ctx();
    // static lay_context* get_top_level_ctx(Widget* w);

    bool is_top_most_enabled();

    void run_context();

    // void set_lay_context(lay_context* context) {
    //     ctx = context; 
    //     id = lay_item(ctx);
    // }
    struct Margins {
        int left;
        int top;
        int right;
        int bottom;
    };

    Margins margins = {};

    void set_margins_ltrb(int left, int top, int right, int bottom) {
        margins = {
            .left = left,
            .top = top,
            .right = right,
            .bottom = bottom,
        };

        if (layout_item) {
            layout_item->set_margins_ltrb(left, top, right, bottom);
        }
        else {
            log_debug("Failed to set margins: layout_item is null");
        }
    }

    void get_margins_ltrb(int* left, int* top, int* right, int* bottom) {
        if (layout_item) {
            lay_scalar l, t, r, b;
            layout_item->get_margins_ltrb(&l, &t, &r, &b);
            *left = l;
            *top = t;
            *right = r;
            *bottom = b;
        }
        else {
            log_debug("Failed to get margins: layout_item is null");
        }
    }

    Widget* get_parent_widget() { return parent_widget; }
    void    set_parent_widget(Widget* pw) { parent_widget = pw; }

    const std::vector<BoxItem*>& get_children() const {
        return children;
    }

    void add_widget(Widget* widget);

    virtual void post_run_context() {
        if (layout_item) {
            lay_vec4 dim = layout_item->get_rect();
            rect.set_left(dim[0]);
            rect.set_top(dim[1]);
            rect.set_width(dim[2]);
            rect.set_height(dim[3]);
        }

        for (BoxItem* child : children) {
            if (Box* b = child->get_layout(); b) {
                b->post_run_context();
            }
        }
    }

    Rect get_rect() {
        //TODO: this could probably be optimized after a resize
        // if (layout_item) {
        //     layout_item->run_context();
        //     lay_vec4 dim = layout_item->get_rect();
        //     rect.set_left(dim[0]);
        //     rect.set_top(dim[1]);
        //     rect.set_width(dim[2]-dim[0]);
        //     rect.set_height(dim[3]-dim[1]);
        // }
        return rect;
    }

    void invalidate(bool send_resize = true);

    Box* get_layout() override { return this; }

    // Will recreate the layout subtree and insert
    // it into the 'parent' view tree
    void create_layout_item_subtree(Box* parent);

    // Will destroy the layout subtree. Before the widget/layout
    // can be used again, create_layout_item_subtree must be called.
    // Application code should not have to use this often, as the
    // take/add_widget functions call these appropriately
    void destroy_layout_item_subtree();

    void remove_subtree_node(Box* node);

    void insert_subtree_node(Box* node);


    void create_layout_item(Box* parent);

    // Release ownership of 'w'. Returns 
    // the widget that was release or null if
    // w as not owned by 'this' or was null.
    Widget* take(Widget* w);

    friend class Widget;
    void rebuild_layout();

    void set_enabled(bool e) { enabled = e; } // TODO: prob some resizing events
    bool is_enabled() const { return enabled; } 

    Box* get_parent_layout();

    int get_behave() const { return behave_flags; }
    int get_contain() const { return contain_flags; }

    void print_view_tree_dimensions(int depth = 0);
    

protected:

    // Will rebuild the entire lay_context no matter which node we call it from
    void rebuild_layout_recursive_helper();

private:
    void update_box();
    void update_box_flags() {
        layout_item->set_contain(contain_flags);
        layout_item->set_behave(behave_flags);
    }

    std::vector<BoxItem*> children;
    Rect         rect = {};
    int          contain_flags = 0;
    int          behave_flags = 0;
    Lay_Item*    layout_item = nullptr;
    Widget*      parent_widget = nullptr;
    bool         dirty = false;
    bool         enabled = false;

};

class VBoxLayout : public Box {
public:

    VBoxLayout(int rows, int cols, Widget* parent = nullptr)
        : Box(rows, cols, parent)
    {
        set_size(rows, cols);
        set_behave(LAY_FILL | LAY_CENTER);
        set_contain(LAY_COLUMN);
    }

    VBoxLayout(Widget* parent = nullptr)
        : VBoxLayout(1,1,parent)
    {
    }
};

class HBoxLayout : public Box {
public:
    HBoxLayout(int rows, int cols, Widget* parent = nullptr)
        : Box(rows, cols, parent)
    {
        set_size(rows, cols);
        set_behave(LAY_FILL | LAY_CENTER);
        set_contain(LAY_ROW);
    }

    HBoxLayout(Widget* parent = nullptr)
        :HBoxLayout(1,1,parent)
    {
    }
};

}
