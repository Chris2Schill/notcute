#include <exception>
#include <notcute/widget.hpp>

#include <notcute/renderer.hpp>
#include <notcute/layout.hpp>
#include <notcute/event_loop.hpp>
#include <notcute/logger.hpp>
#include <notcute/input.hpp>

namespace notcute {

Renderer* Renderer::instance = new Renderer;

std::unordered_map<ncpp::Plane*, std::string> Widget::plane_name_map;

std::unordered_map<Widget*, lay_context*> Widget::layout_ctx_map;

Widget* Widget::focused_widget = nullptr;

Widget::Widget(Widget* parent)
    : Object(parent)
{
    ncplane_options& opts = default_options();
    plane = new ncpp::Plane(opts.rows, opts.cols, opts.x, opts.y);
    if (parent) {
        reparent(parent);
    }
    else {
        // not providing a parent thus being a top level widget means we intend for this widget to be associated with
        // a renderable pile
        ncpp::Pile* pile = Renderer::get_instance()->create_pile_for_widget(this);
        plane->reparent_family(pile);
    }

    set_geometry(Rect{Point{plane->get_x(),plane->get_y()},Size{plane->get_dim_x(), plane->get_dim_y()}});

    EventLoop::get_instance()->subscribe<Event>(this, [this](Event* e) { on_event(e); });
}

void Widget::set_layout(Box* layout) {
    this->box = layout;
    layout->set_parent_widget(this);
    layout->set_enabled(layout->is_top_most_enabled());

    layout->create_layout_item(nullptr);

}

void Widget::set_geometry(const Rect& rect) {
    if (Box* l = get_layout(); l) {
        l->set_size(rect.rows(), rect.cols());
    }
    // EventLoop::get_instance()->post(new DrawEvent(this));
}

bool Widget::on_event(Event* e) {
    switch(e->get_type()) {
        case Event::DRAW:
            return on_draw_event(static_cast<DrawEvent*>(e));
        case Event::RESIZE:
            return on_resize_event(e);
            return true;
        case Event::KEYBOARD_EVENT:
            return on_keyboard_event(static_cast<KeyboardEvent*>(e));
        case Event::MOUSE_EVENT:
            return true;
    }
    return false;
}

void Widget::draw_children() {
    if (Box* layout = get_layout(); layout) {
        for (BoxItem* item : box->get_children()) {
            if (Widget* w = item->get_widget(); w) {
                w->draw();
            }
        }
    }
}

bool Widget::on_draw_event(DrawEvent* e) {
    if (e->get_sender() == this) {
        box->run_context();
        draw();
        return true;
    }
    return false;
}

bool Widget::on_keyboard_event(KeyboardEvent* e) {
    log_debug(fmt::format("keyboard event {}", e->get_key()));
    return false;
}

bool Widget::on_resize_event(Event* e) {
    if (get_layout()->layout_item) {
        get_layout()->layout_item->run_context();
        get_layout()->update_box();
    }
    return true;
    // set_geometry(get_layout()->get_rect());
    // for (BoxItem* b : get_layout()->get_children()) {
    //     if (Widget* w = b->get_widget(); w) {
    //         w->on_resize_event(e);
    //     }
    // }
}

void Widget::show() {

    InputListener input;
    is_showing = true;
    // get_layout()->set_enabled(true);
    while(is_showing) {
        EventLoop::get_instance()->post(new DrawEvent(this));
        EventLoop::get_instance()->process_events();
        Renderer::get_instance()->show(this);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    is_showing = false;
};

}
