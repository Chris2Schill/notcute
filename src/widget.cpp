#include <notcute/widget.hpp>

#include <notcute/renderer.hpp>
#include <notcute/layout.hpp>
#include <notcute/event_loop.hpp>
#include <notcute/logger.hpp>

namespace notcute {

std::unordered_map<ncpp::Plane*, std::string> Widget::plane_name_map;

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

    // EventLoop::get_instance()->subscribe<Event>(this, [this](Event* e) { on_event(e); });
    // EventLoop::get_instance()->subscribe<Event*>(this, &Widget::on_event);
}

void Widget::set_layout(Box* layout) {
    this->box = layout;
    box->set_parent_widget(this);
}

Layout* Widget::get_layout() const { return layout; }

void Widget::set_geometry(const Rect& rect) {
    if (Box* l = get_layout(); l) {
        l->set_size(rect.rows(), rect.cols());
    }
    // EventLoop::get_instance()->post(new DrawEvent(this));
}

// void Widget::on_event(Event* e) {
//     switch(e->get_type()) {
//         case Event::DRAW:
//             on_draw_event(static_cast<DrawEvent*>(e));
//             break;
//         case Event::LAYOUT_REQUEST:
//             break;
//     }
// }

void Widget::draw_children() {
    if (Box* layout = get_layout(); layout) {
        for (BoxItem* item : box->get_children()) {
            if (Widget* w = item->get_widget(); w) {
                w->draw();
            }
        }
    }
}

// void Widget::on_draw_event(DrawEvent* e) {
//     if (e->get_sender() == this) {
//         this->draw(plane);
//     }
// }

void Widget::show() {

    EventLoop::get_instance()->post(new DrawEvent(this));
    is_showing = true;
    while(is_showing) {
        Renderer::get_instance()->show(this);
        EventLoop::get_instance()->process_events();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    is_showing = false;
};

MainWindow::MainWindow() {
    plane = Renderer::get_instance()->create_pile_for_widget(this);
}

}
