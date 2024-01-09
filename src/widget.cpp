#include <exception>
#include <notcute/widget.hpp>

#include <notcute/renderer.hpp>
#include <notcute/layout.hpp>
#include <notcute/event_loop.hpp>
#include <notcute/logger.hpp>
#include <notcute/input.hpp>

namespace notcute {

std::unordered_map<ncpp::Plane*, std::string> Widget::plane_name_map;

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
        // not providing a parent means we intend for this widget to be associated with
        // a renderable pile
        ncpp::Pile* pile = Renderer::get_instance()->create_pile_for_widget(this);
        plane->reparent(pile);
    }

    set_geometry(Rect{Point{plane->get_x(),plane->get_y()},Size{plane->get_dim_x(), plane->get_dim_y()}});

    EventLoop::get_instance()->subscribe<Event>(this, [this](Event* e) { on_event(e); });
    // EventLoop::get_instance()->subscribe<Event*>(this, &Widget::on_event);
}

void Widget::set_layout(Box* layout) {
    this->box = layout;
    box->set_parent_widget(this);
}

void Widget::set_geometry(const Rect& rect) {
    if (Box* l = get_layout(); l) {
        l->set_size(rect.rows(), rect.cols());
    }
    // EventLoop::get_instance()->post(new DrawEvent(this));
}

void Widget::on_event(Event* e) {
    switch(e->get_type()) {
        case Event::DRAW:
            on_draw_event(static_cast<DrawEvent*>(e));
            break;
        case Event::RESIZE:
            on_resize_event(static_cast<ResizeEvent*>(e));
            break;
        case Event::KEYBOARD_EVENT:
            on_keyboard_event(static_cast<KeyboardEvent*>(e));
            break;
    }
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

void Widget::on_draw_event(DrawEvent* e) {
    if (e->get_sender() == this) {
        box->run_context();
        draw();
    }
}

void Widget::on_keyboard_event(KeyboardEvent* e) {
    log_debug(fmt::format("keyboard event {}", e->get_key()));
}

void Widget::on_resize_event(ResizeEvent* e) {
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
    while(is_showing) {
        // get_layout()->run_context();
        EventLoop::get_instance()->post(new DrawEvent(this));
        EventLoop::get_instance()->process_events();
        Renderer::get_instance()->show(this);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    is_showing = false;
};

MainWindow::MainWindow() {
    plane = Renderer::get_instance()->create_pile_for_widget(this);
}

}
