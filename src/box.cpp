#include <notcute/box.hpp>
#include <notcute/widget.hpp>
#include <notcute/event_loop.hpp>
#include <notcute/spacer.hpp>

namespace notcute {


Box::Box(int rows, int cols, Widget* parent) {
    set_size(rows, cols);
    if (parent) {
        set_widget(parent);
        parent->set_layout(this);
    }
}

Box::~Box() {

    log_debug(fmt::format("deleting {}s box", get_widget()->get_name()));
    for (LayoutItem* item : children) {
        delete item;
    }

    delete layout_item;
}

void Box::add_widget(Widget* widget) {
    assert(widget != nullptr);

    widget->reparent(get_widget());
    children.push_back(new WidgetItem(widget));

    insert_subtree_node(widget->get_layout());

    invalidate();
}

void Box::add_widgets(std::vector<notcute::Widget*> ws) {
    for (auto& w : ws) {
        add_widget(w);
    }
}

Widget* Box::take(Widget* w) {
    Widget *taken = nullptr;
    children.erase(
            std::remove_if(
                children.begin(),
                children.end(),
                [w,&taken](LayoutItem* i) { 
                        if (i->get_widget() == w) {
                            taken = w;
                            i->release_ownership();
                            delete i;
                            return true;
                        }
                        return false;
                    }),
            children.end()
        );

    if (taken) {
        remove_subtree_node(taken->get_layout());
    }

    return taken;
}

Spacer* Box::add_spacer() {
    auto spacer = new Spacer(get_widget());
    add_widget(new Spacer);
    return spacer;
}

void Box::set_contain(int flags) {
    contain_flags = flags;
    if (layout_item) {
        layout_item->set_contain(flags);
    }
    else {
        log_debug("Layout lay_item failed to set contain: layout_item is null");
    }
}

void Box::set_behave(int flags) {
    behave_flags = flags;
    if (layout_item) {
        layout_item->set_behave(flags);
    }
    else {
        log_debug("Layout lay_item failed to set contain: layout_item is null");
    }
}

void Box::set_size(int rows, int cols) {
    rect.set_height(rows);
    rect.set_width(cols);

    if (layout_item) {
        layout_item->set_size(rows, cols);
    }
}

void Box::set_width(int cols) {
    set_size(rect.rows(), cols);
}

void Box::set_height(int rows) {
    set_size(rows, rect.cols());
}

Rect Box::get_rect() {
    return rect;
}

Widget* Box::get_widget() {
    return its_widget;
}

void Box::set_margins_ltrb(int left, int top, int right, int bottom) {
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

void Box::get_margins_ltrb(int* left, int* top, int* right, int* bottom) {
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

const std::vector<LayoutItem*>& Box::get_children() const {
    return children;
}

void Box::create_layout_item(Box* parent = nullptr) {
    if (layout_item) {
        delete layout_item;
    }

    if (parent) {
        layout_item = new Lay_Item(*parent->layout_item);
    }
    else {
        layout_item = new Lay_Item(rect.rows(), rect.cols());
    }
}

Box* Box::get_parent_layout() {
    Widget* w = get_widget();
    if (!w) { return nullptr; }

    Widget* pw = w->get_parent();
    if (!pw) { return nullptr; }

    return pw->get_layout();
}

void Box::create_layout_item_subtree(Box* parent) {
    create_layout_item(parent);
    // log_debug(fmt::format("recreated lay_item: id = {}", layout_item->id));

    // parent will be null for top level widgets
    if (parent) {
        parent->layout_item->insert(layout_item);
    }

    for (LayoutItem* child : children) {
        if (Box* cb = child->get_layout(); cb) {
            cb->create_layout_item_subtree(this);
        }
    }
}

void Box::destroy_layout_item_subtree() {
    // Delete the widgets Lay_Item and and all of its childrens as well
    for (LayoutItem* child : children) {
        if (Box* b = child->get_layout(); b) {
            b->destroy_layout_item_subtree();
        }
    }

    delete layout_item;
    layout_item = nullptr;
}

void Box::insert_subtree_node(Box* node) {
    assert(node != nullptr);
    node->destroy_layout_item_subtree();
    node->create_layout_item_subtree(this);
}

void Box::remove_subtree_node(Box* node) {
    node->destroy_layout_item_subtree();
    node->get_widget()->reparent(nullptr);
    node->create_layout_item_subtree(nullptr); // nullptr parents means make this subtree now a standlone tree with node as the root
    rebuild_layout();
}

void Box::invalidate(bool send_resize) {
    log_debug("invalidate() " + get_widget()->get_name());
    layout_item->set_size(rect.rows(), rect.cols());
    layout_item->set_margins_ltrb(margins.left, margins.top, margins.right, margins.bottom);
    layout_item->set_contain(contain_flags);
    layout_item->set_behave(behave_flags);

    for (LayoutItem* child : children) {
        if (Box* b = child->get_layout(); b) {
            b->invalidate(false);
        }
    }

    if (send_resize) {
        EventLoop::get_instance()->post(new Event(get_widget(), Event::RESIZE));
    }
    get_widget()->redraw();// TODO: needed? or just fix resize
}

void Box::rebuild_layout() {
    Widget* top_level = its_widget->get_top_level_widget();
    Box* layout = top_level->get_layout();
    layout->destroy_layout_item_subtree();
    layout->create_layout_item_subtree(nullptr);
    layout->invalidate();
}

void Box::run_context() {
    if (layout_item) {
        layout_item->run_context();
        post_run_context();
    }
    else {
        log_debug("Failed to run context: layout_item is null");
    }
}

void Box::post_run_context() {
    if (layout_item) {
        lay_vec4 dim = layout_item->get_rect();
        rect.set_left(dim[0]);
        rect.set_top(dim[1]);
        rect.set_width(dim[2]);
        rect.set_height(dim[3]);
    }

    for (LayoutItem* child : children) {
        if (Box* b = child->get_layout(); b) {
            b->post_run_context();
        }
    }
}

void Box::print_view_tree_dimensions(int depth) {
    std::stringstream ss;
    for (int i = 0; i < depth; ++i) {
        ss << " |";
    }

    lay_vec4 r = layout_item->get_rect();
    Rect rect {
        Point{r[0], r[1]},
        Size{(unsigned)r[2], (unsigned)r[3]},
    };

    ss << "-" << get_widget()->get_name();
    ss << " = " << rect.to_string();

    notcute::log_debug(ss.str());

    for (LayoutItem* child : get_children()) {
        if (Box* l = child->get_layout(); l) {
            l->print_view_tree_dimensions(depth+1);
        }
    }
}

void Box::update_box() {
    // TODO: update_box does not need to happen until a redraw/resize event.
    // Currently is happens on every insertion/deletion of a view tree
    // log_debug("update_box() " + get_widget()->get_name());
    if (layout_item) {
        lay_vec4 r = layout_item->get_rect();
        rect = Rect {
            Point{r[0],r[1]},
                Size{(unsigned)r[2],(unsigned)r[3]},
        };

        for (LayoutItem* child : children) {
            if (Box* b = child->get_layout(); b) {
                b->update_box();
            }
        }
    }
}


}
