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
        plane->reparent(pile);
    }

    set_geometry(Rect{Point{plane->get_x(),plane->get_y()},Size{plane->get_dim_x(), plane->get_dim_y()}});

    ncpp::Cell c(' ');
    plane->set_base_cell(c);

    redraw();
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
        event_loop->process_events();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    is_showing = false;
    event_loop->set_showing_widget(prev_showing_widget);
    Widget::focused_widget = prev_focused_widget;
};

void Widget::done_showing() {
    is_showing = false;
}

void Widget::set_layout(Layout* layout) {
    this->layout = layout;
    layout->set_widget(this);
    layout->create_layout_item(nullptr);
}

void Widget::pre_draw(ncpp::Plane* plane) {
    plane->erase();

    //Note: Margins are already accounted for by the box model

    Rect rect = layout->get_rect();

    // If we are nested inside of a child widget other than the top level widget
    // then we must translate the relative coords to global coords for positioning
    if (parent) {
        parent->get_plane()->translate_abs(&rect.m_pos.y, &rect.m_pos.x);
    }

    plane->resize(rect.height(), rect.width());
    plane->move(rect.y(), rect.x());
    plane->resize_realign();

    if (layout) {
        for (LayoutItem* item : layout->get_children()) {
            if (Widget* w = item->get_widget(); w) {
                w->pre_draw(w->get_plane());
            }
        }
    }
}

void Widget::draw(ncpp::Plane* plane) {
    draw_children();
}

void Widget::redraw() {
    if (!dirty) {
        dirty = true;
        EventLoop::get_instance()->post(new DrawEvent(this));
    }
}

void Widget::redraw_all() {
    Widget* toplevel = get_top_level_widget();
    toplevel->redraw();
}

ncpp::Plane* Widget::get_plane() {
    return plane;
}

Layout* Widget::get_layout() {
    return layout;
}

Widget* Widget::get_parent() {
    return parent;
}

void Widget::reparent(Widget* new_parent) {
    parent = new_parent;
    if (new_parent) {
        plane->reparent_family(new_parent->plane);
    }
    else {
        plane->reparent_family((ncpp::Plane*)nullptr);
    }
}

bool Widget::on_event_start(Event* e) {
    if (on_event(e)) {
        return true;
    }

    if (layout) {
        for (int i = 0; i < layout->children.size(); ++i) {
            if (Widget* child = layout->children[i]->get_widget()) {
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

bool Widget::on_draw_event(DrawEvent* e) {
    if (dirty) {
        notcute::log_debug(fmt::format("redraw {}", get_name()));

        // Update the layout viewtree geometry, this only needs
        // to happen once for a redraw of a particular view tree
        // whether it is a subtree or not
        if (e->get_sender() == this) {
            layout->run_context();
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

    // TODO: Should keyboard events propagate up the view
    // tree if they are not handled?
    // if (auto p = get_parent(); p) {
    //     return p->on_keyboard_event(e);
    // }

    // TODO: Maybe make this an option? It seemed
    // nice during testing, but is probalby unecessary. Maybe
    // dedicate the standard ctrl-l key combo to redraw all
    redraw_all();

    return false;
}

bool Widget::on_resize_event(Event* e) {
    if (layout->layout_item) {
        layout->layout_item->run_context();
        layout->update_box();
    }
    return true;
}

std::string Widget::to_string() const {
    return "Widget " + get_name();
}

void Widget::set_focus_policy(FocusPolicy fp) {
    focus_policy = fp;
}

FocusPolicy Widget::get_focus_policy() const {
    return focus_policy;
}

void Widget::set_focus() {
    if (focus_policy == FocusPolicy::FOCUS) {
        if (focused_widget != this) {
            Widget* prev_focused_widget = focused_widget;
            focused_widget = this;

            if (prev_focused_widget) {
                prev_focused_widget->focus_state_changed(false);
            }

            focused_widget->focus_state_changed(true);
        }
    }
    else {
        log_debug(fmt::format("Tried to set focus on {} but focus policy is NO_FOCUS", get_name()));
    }
}

bool Widget::is_focused() {
    return focused_widget == this;
}

void Widget::set_geometry(const Rect& rect) {
    if (layout) {
        layout->set_size(rect.rows(), rect.cols());
    }

    // Even though pre_draw() will resize and move the
    // plane according to the box layout model, we update
    // the planes size and position here so that its data
    // is always up to date. This is mainly for complex widgets
    // that manipulate multiple (potentially offscreen/unparented) planes
    // behind the scenes to accomplish their rendering goals...(like ScrollArea)
    plane->resize(rect.rows(), rect.cols());
    plane->move(rect.y(), rect.x());
    plane->resize_realign();
}

Rect Widget::get_geometry() {
    return layout->get_rect();
}

bool Widget::is_top_level() const {
    return parent == nullptr;
}

Widget* Widget::get_top_level_widget() {
    Widget* w = this;
    while (!w->is_top_level()) {
        w = w->get_parent();
    }
    return w;
}

Widget* Widget::take() {
    assert(parent != nullptr);
    Widget* wid = parent->get_layout()->take(this);
    assert(wid == this);
    return wid;
}

void Widget::delete_later() {
    assert(parent == nullptr);
    EventLoop::get_instance()->post(new Event(this, Event::DELETE_LATER));
}

void Widget::take_and_delete_later() {
    take();
    delete_later();
}


void Widget::set_fg_color(Color c) {
    plane->set_fg_rgb(c.to_channel());
}

void Widget::set_bg_color(Color c) {
    plane->set_bg_rgb(c.to_channel());
}

void Widget::set_name(const std::string& name) {
    plane_name_map[plane] = name;
}

const std::string& Widget::get_name() const {
    static const std::string NO_NAME = "(no_name)";
    const std::string& name = plane_name_map[plane];
    return name.empty() ? NO_NAME : name;
}

void Widget::debug_set_plane_color(int r, int g, int b) {
    ncpp::Cell c(' ');
    c.set_bg_rgb8(r,g,b);
    // c.set_fg_rgb8(r,g,b);
    plane->set_base_cell(c);
    redraw();
}

Widget*& Widget::get_focused_widget() {
    return focused_widget;
}

ncplane_options& Widget::default_options() {
    auto& nc = ncpp::NotCurses::get_instance();
    unsigned rows, cols;
    nc.get_term_dim(&rows, &cols);

    static ncplane_options opts = {
        .y = 0,
        .x = 0,
        .rows = 1,
        .cols = 1,
    };

    return opts;
}

void Widget::draw_children() {
    if (layout) {
        for (LayoutItem* item : layout->get_children()) {
            if (Widget* w = item->get_widget(); w) {
                w->draw(w->get_plane());
                // w->dirty = false;
            }
            else if (Layout* b = item->get_layout(); b){
                // TODO: draw any widgets inside of a layout that does not
                // have its own widget
            }
        }
    }
}

}
