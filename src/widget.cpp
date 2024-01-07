#include <ncqt/widget.hpp>

#include <ncqt/renderer.hpp>
#include <ncqt/layout.hpp>
#include <ncqt/event_loop.hpp>
#include <ncqt/logger.hpp>

namespace ncqt {

std::unordered_map<ncpp::Plane*, std::string> Widget::plane_name_map;

Widget::Widget(Widget* parent)
    : Object(parent)
{
    // not providing a parent means we intend for this widget to be associated with
    // a renderable pile
    // if (!parent) {
    //     // plane = Renderer::get_instance()->create_pile_for_widget(this);
    //
    // }
    // else {

    ncplane_options& opts = default_options();
    plane = new ncpp::Plane(opts.rows, opts.cols, opts.x, opts.y);
    if (parent) {
        reparent(parent);
    }
    else {
        ncpp::Pile* pile = Renderer::get_instance()->create_pile_for_widget(this);
        plane->reparent(pile);
    }
    EventLoop::get_instance()->post(new DrawEvent(this));

    // }

    set_geometry(Rect{Point{plane->get_x(),plane->get_y()},Size{plane->get_dim_x(), plane->get_dim_y()}});

    EventLoop::get_instance()->subscribe<Event>(this, [this](Event* e) { on_event(e); });
    // EventLoop::get_instance()->subscribe<Event*>(this, &Widget::on_event);
}

void Widget::set_layout(Layout* layout) {
    this->layout = layout;
    layout->set_parent_widget(this);
    layout->invalidate();
}
Layout* Widget::get_layout() const { return layout; }

void Widget::set_geometry(const Rect& rect) {
    // if (get_layout()) {
    //     // get_layout()->set_geometry(rect);
    //     log_debug(fmt::format("{} Resizing widget: {}", get_layout()->debug_name, rect.to_string()));
    // }
    // else {
    //     log_debug(fmt::format("null layout Resizing widget: {}", rect.to_string()));
    // }
    // if (Layout* l = get_layout(); l) {
    //     l->invalidate();
    // }
    log_debug(fmt::format("{} moved to ({},{})", get_name(), rect.y(), rect.x()));
    plane->resize(rect.height(), rect.width());
    plane->move(rect.y(), rect.x());
    plane->resize_realign();
}

void Widget::on_event(Event* e) {
    switch(e->get_type()) {
        case Event::DRAW:
            on_draw_event(static_cast<DrawEvent*>(e));
            break;
        case Event::LAYOUT_REQUEST:
            break;
    }
}

void Widget::on_draw_event(DrawEvent* e) {
    if (e->get_sender() == this) {
        this->draw(plane);
    }
}

void Widget::show() {

    is_showing = true;
    while(is_showing) {
        Renderer::get_instance()->show(this);
        EventLoop::get_instance()->process_events();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    is_showing = false;
};

Rect Widget::get_contents_rect() const {
    if (auto l = get_layout(); l) {
        return l->get_geometry();
    }
    return get_geometry(); // TODO subtract margins
    // Rect rect = {};
    // if (Layout* l = get_layout(); l) {
    //     for (auto item : l->get_items()) {
    //         recqwidget.cppt.y += 
    //     }
    // }
    // return rect;
}

MainWindow::MainWindow() {
    plane = Renderer::get_instance()->create_pile_for_widget(this);
}

}
