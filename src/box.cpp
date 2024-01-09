#include <notcute/box.hpp>
#include <notcute/widget.hpp>
#include <notcute/event_loop.hpp>

namespace notcute {

WidgetItem::~WidgetItem() {
    delete wid;
}

Box::Box(Widget* parent)
    : Box(parent ? parent->get_layout() : nullptr)
{
}

VBoxLayout::VBoxLayout(Widget* parent)
    : Box(parent ? parent->get_layout() : nullptr) {
}

HBoxLayout::HBoxLayout(Widget* parent)
    : Box(parent ? parent->get_layout() : nullptr)
{
}

void Box::add_widget(Widget* widget) {
    children.push_back(new WidgetItem(widget));
    if (Box* b = widget->get_layout(); b) {
        insert(b);
    }
}

Box::~Box() {

    log_debug(fmt::format("deleting {}s box", get_parent_widget()->get_name()));
    for (BoxItem* item : children) {
        delete item;
    }

    if (get_parent_widget()->is_top_level()) {
        log_debug(fmt::format("deleting {}s box context", get_parent_widget()->get_name()));
        lay_destroy_context(ctx);
    }
}

void Box::run_context() {
    // lay_run_item(ctx, id);
    lay_run_context(ctx);
    // if (Widget* w = get_parent_widget(); w) {
    //     ResizeEvent event(w, get_rect());
    //     w->on_resize_event(&event);
    // }
}

}
