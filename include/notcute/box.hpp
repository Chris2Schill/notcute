#pragma once

#include <vector>
#include "rect.hpp"
#include "layout.hpp"
#include "lay_item.hpp"
#include "layout_item.hpp"

namespace notcute {

class Widget;
class Box;
class Spacer;

class BoxLayout : public LayoutItem {
public:
    struct Margins {
        int left;
        int top;
        int right;
        int bottom;
    };

    BoxLayout(int rows, int cols, Widget* parent = nullptr);
    BoxLayout(Widget* parent = nullptr) : BoxLayout(1,1, parent) {}
    ~BoxLayout();

    // Add widget(s) to the layout. The layout will then
    // own the widget. Any widgets owned by the layout will be
    // deleted when the Layout is deleted.
    void add_widget(Widget* widget);
    void add_widgets(std::vector<notcute::Widget*> wids);

    // Release ownership of 'w' and removes it from
    // the layout.
    // Returns the widget that was released or null if
    // w as not owned by 'this' or was null.
    Widget* take(Widget* w);

    // Convenience function to add a spacer widget to the layouts children.
    // Use spacers to force elements in a particular direction if they don't happen
    // to fully fill their parent layout's geometry.
    Spacer* add_spacer();

    // Sets how the layout will organize its elements.
    // LAY_ROW = horizontally
    // LAY_COLUMN = vertically
    // LAY_LAYOUT = free layout
    // LAY_FLEX = lay items in a row until width of layout and then start on the next row
    void set_contain(int flags);
    uint32_t get_contain() const { return contain_flags; }

    // Sets how the layout will behave in the context of its parent layout.
    // LAY_CENTER, LAY_VCENTER, LAY_HCENTER,
    // LAY_LEFT, LAY_RIGHT
    // LAY_LAYOUT = free layout
    void set_behave(int flags);
    uint32_t get_behave() const { return behave_flags; }

    void set_size(int rows, int cols);
    void set_width(int cols);
    void set_height(int rows);
    Rect get_rect();

    Widget* get_widget() override;

    void set_margins_ltrb(int left, int top, int right, int bottom);
    void get_margins_ltrb(int* left, int* top, int* right, int* bottom);

    const std::vector<LayoutItem*>& get_children() const;

    // Will run the Lay_Item context, i.e. calculating the positions and sizes
    // of all the children. Will calculate for entire view tree no matter
    // which node this is called on
    void run_context();

    // Reflect the changes in the box model to the local layout geometry.
    // Note: I've allowed two "sources of truth" when it comes to position and sizes in the
    // box model. It allows client code to be a bit lazier when it comes to defining its
    // positions, layouts, etc, not having to define everything in a specific order
    virtual void post_run_context();

    // Used to tell if a LayoutItem is a Layout or not.
    Layout* get_layout() override { return this; }

    // Creates a Lay_Item for this layout. The created Lay_Item
    // will be a child of 'parent's Lay_Item
    void create_layout_item(BoxLayout* parent);

    // Will recreate the layout subtree and insert
    // it into the 'parent' view tree
    void create_layout_item_subtree(BoxLayout* parent);

    // Will destroy the layout subtree. Before the widget/layout
    // can be used again, create_layout_item_subtree must be called.
    // Application code should not have to use this often, as the
    // take/add_widget functions call these appropriately
    void destroy_layout_item_subtree();

    void insert_subtree_node(BoxLayout* node);
    void remove_subtree_node(BoxLayout* node);

    // Called when the underlying lay_item view tree is
    // modified. Should update the layouts stored geometry
    // according to the latest box model updates
    void invalidate(bool send_resize = true);

    // Rebuilds the Lay_Item viewtree. Will rebuild the entire tree
    // starting from the top_level layout. This is due to a current
    // limitation of the Lay_Item box model.
    void rebuild_layout();

    // Returns the parent layout if it exists and nullptr otherwise.
    BoxLayout* get_parent_layout();

    // Prints Debug information
    void print_view_tree_dimensions(int depth = 0);

    friend class Widget;


private:
    // Will rebuild the entire lay_context no matter which node we call it from
    void rebuild_layout_recursive_helper();

    // Used internally, from application code point of view, you
    // set the layout of a widget not the widget of a layout
    void set_widget(Widget* pw) { its_widget = pw; }

    void update_box();

    std::vector<LayoutItem*> children;
    Rect                  rect = {};
    Margins               margins = {};
    uint32_t              contain_flags = 0;
    uint32_t              behave_flags = 0;
    Lay_Item*             layout_item = nullptr;
    Widget*               its_widget = nullptr;
};

class VBoxLayout : public BoxLayout {
public:

    VBoxLayout(int rows, int cols, Widget* parent = nullptr)
        : BoxLayout(rows, cols, parent)
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

class HBoxLayout : public BoxLayout {
public:
    HBoxLayout(int rows, int cols, Widget* parent = nullptr)
        : BoxLayout(rows, cols, parent)
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
