#include <exception>
#include <ncpp/NotCurses.hh>
#include <notcurses/notcurses.h>
#include <notcute/widget.hpp>

#include <notcute/renderer.hpp>
#include <notcute/layout.hpp>
#include <notcute/event_loop.hpp>
#include <notcute/logger.hpp>
#include <notcute/input.hpp>

namespace notcute {

Renderer* Renderer::instance = new Renderer;

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
        // not providing a parent thus being a top level widget means we intend for this widget to be associated with
        // a renderable pile
        ncpp::Pile* pile = Renderer::get_instance()->create_pile_for_widget(this);
        plane->reparent_family(pile);
    }

    set_geometry(Rect{Point{plane->get_x(),plane->get_y()},Size{plane->get_dim_x(), plane->get_dim_y()}});

    // plane->set_bg_alpha(NCALPHA_HIGHCONTRAST);
    ncpp::Cell c(' ');
    notcurses* nc = ncpp::NotCurses::get_instance();
    uint32_t fg, bg;
    notcurses_default_foreground(nc, &fg);
    notcurses_default_background(nc, &bg);
    // uint64_t channels = ((uint64_t)fg << 32) + (uint64_t)bg;
    // c.set_fg_rgb(fg);
    c.set_bg_rgb(bg);
    plane->set_base_cell(c);
    // plane->set_fg_alpha(NCALPHA_HIGHCONTRAST);
    // plane->set_fg_palindex(255);

    redraw();
    // EventLoop::get_instance()->subscribe<Event>(this, [this](Event* e) { on_event(e); });
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
    get_plane()->move(rect.y(), rect.x());
    // EventLoop::get_instance()->post(new DrawEvent(this));
}

bool Widget::on_event_start(Event* e) {
    if (on_event(e)) {
        return true;
    }

    if (box) {
        for (int i = 0; i < box->children.size(); ++i) {
            if (Widget* child = box->children[i]->get_widget()) {
                if (child->on_event_start(e)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Widget::on_event(Event* e) {
    switch(e->get_type()) {
        case Event::DRAW:
            return on_draw_event(static_cast<DrawEvent*>(e));
        case Event::RESIZE:
            return on_resize_event(e);
        case Event::KEYBOARD_EVENT:
            return on_keyboard_event(static_cast<KeyboardEvent*>(e));
        case Event::MOUSE_EVENT:
            return true;
        default:
            return false;
    }
}

void Widget::draw_children() {
    if (Box* layout = get_layout(); layout) {
        for (BoxItem* item : box->get_children()) {
            if (Widget* w = item->get_widget(); w) {
                w->draw(w->get_plane());
            }
            else if (Box* b = item->get_layout(); b){
                // TODO: draw any widgets inside of a layout that does not
                // have its own widget
            }
        }
    }
}

bool Widget::on_draw_event(DrawEvent* e) {
    if (dirty) {
        // notcute::log_debug(fmt::format("redraw {}", get_name()));

        // Update the layout viewtree geometry, this only needs
        // to happen once for a redraw of a particular view tree
        // whether it is a subtree or not
        if (e->get_sender() == this) {
            box->run_context();
        }
        
        // drill down the view tree, move the planes into position
        // and command the widgets to draw to their owning planes
        pre_draw(plane);
        draw(plane);

        // Tells notcurses to render the planes to the terminal.
        // This only needs to happen once per redraw of a viewtree.
        // We use the top level widget because we've allowed
        // any widget without a parent to potentially be allocated
        // its own ncpile. Only one ncpile can be rendered to the
        // terminal at a time.
        if (e->get_sender() == this) {
            Renderer::get_instance()->show(get_top_level_widget());
        }

        dirty = false;
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

void Widget::redraw() {
    dirty = true;
    EventLoop::get_instance()->post(new DrawEvent(this));
}

void Widget::show() {
    InputListener input;

    EventLoop* event_loop = EventLoop::get_instance();

    Widget* prev_focused_widget = Widget::focused_widget;
    Widget* prev_showing_widget = event_loop->get_showing_widget();

    event_loop->set_showing_widget(this);
    is_showing = true;
    set_focus();

    redraw();

    while(is_showing) {
        redraw();
        event_loop->process_events();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    is_showing = false;
    event_loop->set_showing_widget(prev_showing_widget);
    Widget::focused_widget = prev_focused_widget;
};

}
