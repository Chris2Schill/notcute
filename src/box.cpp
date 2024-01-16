#include <notcute/box.hpp>
#include <notcute/widget.hpp>
#include <notcute/event_loop.hpp>

namespace notcute {

WidgetItem::~WidgetItem() {
    delete wid;
}

void WidgetItem::release_ownership() {
    wid = nullptr;
}

Box::Box(int rows, int cols, Widget* parent) {
    set_size(rows, cols);
    if (parent) {
        set_parent_widget(parent);
        parent->set_layout(this);
    }
}

Box* WidgetItem::get_layout() { return wid->get_layout(); }

void Box::insert_subtree_node(Box* node) {
    assert(node != nullptr);
    // if (!node->layout_item) {
    node->destroy_layout_item_subtree();
    // rebuild_layout();
    // node->get_widget()->reparent(get_widget());
    node->create_layout_item_subtree(this);
    // layout_item->insert(node->layout_item);
    // }
    // else {
    //     layout_item->insert(widget->get_layout()->layout_item);
    // }
}

void Box::add_widget(Widget* widget) {
    assert(widget != nullptr);

    widget->reparent(get_parent_widget());
    children.push_back(new WidgetItem(widget));

    insert_subtree_node(widget->get_layout());

    invalidate();
}

Box::~Box() {

    log_debug(fmt::format("deleting {}s box", get_parent_widget()->get_name()));
    for (BoxItem* item : children) {
        delete item;
    }

    delete layout_item;
}

void Box::update_box() {
    // TODO: update_box does not need to happen until a redraw/resize event.
    // Currently is happens on every insertion/deletion of a view tree
    // log_debug("update_box() " + get_parent_widget()->get_name());
    if (layout_item) {
        lay_vec4 r = layout_item->get_rect();
        rect = Rect {
            Point{r[0],r[1]},
                Size{(unsigned)r[2],(unsigned)r[3]},
        };

        for (BoxItem* child : children) {
            if (Box* b = child->get_layout(); b) {
                b->update_box();
            }
        }
    }
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
        // Widget::layout_ctx_map[get_widget()] = layout_item->get_context();
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
    // get_widget()->reparent(parent->get_widget());

    create_layout_item(parent);
    // log_debug(fmt::format("recreated lay_item: id = {}", layout_item->id));
    // parent will be null for top level widgets
    if (parent) {
        parent->layout_item->insert(layout_item);
    }

    for (BoxItem* child : children) {
        if (Box* cb = child->get_layout(); cb) {
            cb->create_layout_item_subtree(this);
        }
    }
}


void Box::invalidate(bool send_resize) {
    log_debug("invalidate() " + get_parent_widget()->get_name());
    layout_item->set_size(rect.rows(), rect.cols());
    layout_item->set_margins_ltrb(margins.left, margins.top, margins.right, margins.left);
    layout_item->set_contain(contain_flags);
    layout_item->set_behave(behave_flags);

    for (BoxItem* child : children) {
        if (Box* b = child->get_layout(); b) {
            b->invalidate(false);
        }
    }

    if (send_resize) {
        EventLoop::get_instance()->post(new Event(get_parent_widget(), Event::RESIZE));
    }
    get_widget()->redraw();// TODO: needed? or just fix resize
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

Widget* Box::take(Widget* w) {
    Widget *taken = nullptr;
    children.erase(
            std::remove_if(
                children.begin(),
                children.end(),
                [w,&taken](BoxItem* i) { 
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

void Box::remove_subtree_node(Box* node) {
    node->destroy_layout_item_subtree();
    node->get_widget()->reparent(nullptr);
    node->create_layout_item_subtree(nullptr); // nullptr parents means make this subtree now a standlone tree with node as the root
    node->get_layout()->set_enabled(false);
    rebuild_layout();
}

void Box::destroy_layout_item_subtree() {
    // Delete the widgets Lay_Item and and all of its childrens as well
    for (BoxItem* child : children) {
        if (Box* b = child->get_layout(); b) {
            b->destroy_layout_item_subtree();
        }
    }

    delete layout_item;
    layout_item = nullptr;
}

bool Box::is_top_most_enabled() {
    Widget* top_level = get_parent_widget()->get_top_level_widget();
    return top_level->get_layout()->is_enabled();
}

void Box::rebuild_layout() {
    Widget* top_level = parent_widget->get_top_level_widget();
    top_level->get_layout()->destroy_layout_item_subtree();
    top_level->get_layout()->create_layout_item_subtree(nullptr);
    top_level->get_layout()->invalidate();

    // for (BoxItem* child : top_level->get_layout()->get_children()) {
    //     if (Box* l = child->get_layout(); l) {
    //         l->create_layout_item_subtree(top_level->get_layout());
    //     }
    // }
    // top_level->get_layout()->rebuild_layout_recursive_helper(top_level->get_layout());
    // top_level->get_layout()->rebuild_layout_recursive_helper();
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

    for (BoxItem* child : get_children()) {
        if (Box* l = child->get_layout(); l) {
            // Widget* w = l->get_parent_widget();
            // notcute::log_debug("rebuilding: " + w->get_name());
            l->print_view_tree_dimensions(depth+1);
        }
    }
    
}

// void Box::rebuild_layout_recursive_helper() {
//
//     notcute::log_debug("rebuilding: " + get_parent_widget()->get_name());
//     get_parent_widget()->create_layout_item();
//
//     for (BoxItem* child : get_children()) {
//         if (Box* l = child->get_layout(); l) {
//             // Widget* w = l->get_parent_widget();
//             // notcute::log_debug("rebuilding: " + w->get_name());
//             l->rebuild_layout_recursive_helper();
//         }
//     }
//
//     // layout_item = new Lay_I
//     //
//     // for (BoxItem* child : children) {
//     //     if (Box* l = child->get_layout(); l) {
//     //         rebuild_layout_recursive_helper(l);
//     //     }
//     // }
//
// }

// lay_context* Box::get_top_level_ctx() {
//     return get_top_level_ctx(parent_widget);
// }

// lay_context* Box::get_top_level_ctx(Widget* w) {
//     auto iter = Widget::layout_ctx_map.find(w);
//     if (iter == Widget::layout_ctx_map.end()) {
//         return nullptr;
//     }
//     return iter->second;
// }

}
