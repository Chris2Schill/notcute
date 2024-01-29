#include <notcute/widget.hpp>

#include <ncpp/NotCurses.hh>
#include <notcurses/notcurses.h>

#include <notcute/renderer.hpp>
#include <notcute/layout.hpp>
#include <notcute/event_loop.hpp>
#include <notcute/logger.hpp>
#include <notcute/input.hpp>
#include <notcute/focus_stack.hpp>

namespace notcute {

Renderer* Renderer::instance = new Renderer;

std::unordered_map<ncpp::Plane*, std::string> Widget::plane_name_map;
int Widget::alive_widgets = 0;

Widget* Widget::focused_widget = nullptr;
Point Widget::mouse_position = {-1,-1};

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
    alive_widgets++;
}

void Widget::show() {
    InputListener input;

    EventLoop* event_loop = EventLoop::get_instance();

    Widget* prev_focused_widget = Widget::focused_widget;
    Widget* prev_showing_widget = event_loop->get_showing_widget();

    event_loop->set_showing_widget(this);
    is_showing = true;

    if (get_attribute(ATTR_FOCUS_ON_SHOW)) {
        set_focus();
    }

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

void Widget::post_draw(ncpp::Plane* plane) {
    if (layout) {
        for (LayoutItem* item : layout->get_children()) {
            if (Widget* w = item->get_widget(); w) {
                w->post_draw(w->get_plane());
            }
        }
    }
}

void Widget::redraw() {
    if (!dirty) {
        set_dirty();
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
        ncplane_move_family_above(plane->to_ncplane(), new_parent->plane->to_ncplane());
    }
    else {
        plane->reparent_family((ncpp::Plane*)nullptr);
        ncplane_move_family_bottom(plane->to_ncplane());
    }
}

bool Widget::on_event(Event* e) {
    switch(e->get_type()) {
        case Event::DRAW:
            return on_draw_event(static_cast<DrawEvent*>(e));
        case Event::RESIZE:
            return on_resize_event(e);
        case Event::KEYBOARD_EVENT:
            return on_keyboard_event(static_cast<KeyboardEvent*>(e));
        case Event::MOUSE_EVENT: {
            auto me = static_cast<MouseEvent*>(e);
            if (me->get_button() == NCKEY_BUTTON1 &&
                me->get_evtype() == NCTYPE_PRESS &&
                get_attribute(ATTR_MOUSE_EVENTS_ENABLED)) {
                set_focus();
                return on_mouse_event(me);
            }
            return false;
        }
        case Event::FOCUS_IN: {
            bool handled = on_focus_in_event(static_cast<FocusEvent*>(e));
            focus_changed(true);
            log_debug(fmt::format("{} gained focus", get_name()));
            return handled;
        }
        case Event::FOCUS_OUT: {
            bool handled  = on_focus_out_event(static_cast<FocusEvent*>(e));
            focus_changed(false);
            log_debug(fmt::format("{} lost focus", get_name()));
            return handled;
        }
        default:
            return false;
    }
}

bool Widget::on_draw_event(DrawEvent* e) {
    if (!layout) {
        return false;
    }

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
        dirty = false;
        post_draw(plane);

        // Tells notcurses to render the planes to the terminal.
        // This only needs to happen once per redraw of a viewtree.
        // We use the top level widget because we've allowed
        // any widget without a parent to potentially be allocated
        // its own ncpile. Only one ncpile can be rendered to the
        // terminal at a time.
        if (e->get_sender() == this) {
            Renderer::get_instance()->show(get_top_level_widget());
        }

        return true;
    }
    return false;
}

bool Widget::on_keyboard_event(KeyboardEvent* e) {
    log_debug(fmt::format("keyboard event {}", e->get_key()));

    // If sublass did not handle 'e' and they forwarded 'e' to us
    // then give it to our parent
    // if (focus_policy == FocusPolicy::FOCUS) {
    //     if (auto p = get_parent(); p) {
    //         if ( p->on_keyboard_event(e)) {
    //             return true;
    //         }
    //     }
    // }

    if (e->get_key() == NCKEY_TAB && parent) {
        parent->focus_next_in_chain();
    }

    // if (e->get_key() == NCKEY_TAB && parent) {
    //     Widget* prnt = parent;
    //
    //     while(prnt)
    //     {
    //         if (prnt->focus_next_in_chain()) {
    //             return true;
    //         }
    //         else {
    //             prnt = prnt->parent;
    //         }
    //     }
    //
    // }

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

bool Widget::on_mouse_event(MouseEvent* e) {
    log_debug(fmt::format("mouse event {}", e->get_button()));


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
                EventLoop::get_instance()->post(new FocusEvent(Event::FOCUS_OUT, this, prev_focused_widget));
            }
        }
        EventLoop::get_instance()->post(new FocusEvent(Event::FOCUS_IN, this, this));
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
    EventLoop::get_instance()->post(new Event(Event::DELETE_LATER, this, this));
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

Widget* Widget::get_deepest_widget_under_mouse() {
    Widget* found = nullptr;
    if (layout) {
        for (LayoutItem* child : layout->get_children()) {
            if (Widget* cw = child->get_widget(); cw) {
                if (Widget* wid = cw->get_deepest_widget_under_mouse(); wid) {
                    if (!found) {
                        found = wid;
                    }
                }
            }
        }
    }

    // Letting the recursion finish through the view tree ensures on_hover_leave
    // is called when it needs to be. thats why we don't break out of the loop.
    if (found) { return found; }

    // Maybe we should make a separate bool to check for if a widget
    // receives mouse events, for now use focus_policy
    if (focus_policy == FocusPolicy::FOCUS && is_under_mouse()) {
        if (!is_hovered) {
            is_hovered = true;
            on_hover_enter();
        }
        return this;
    }

    if (is_hovered) {
        is_hovered = false;
        on_hover_leave();
    }

    return nullptr;
}

bool Widget::is_under_mouse() {
    Point mouse_pos = mouse_position;
    Rect r = get_geometry();
    int y = plane->get_abs_y();
    int x = plane->get_abs_x();

    if (x <= mouse_pos.x && mouse_pos.x <= r.right() &&
        y <= mouse_pos.y && mouse_pos.y <= r.bottom()) {
        log_debug(fmt::format("widget {} under mouse", get_name()));
        return true;
    }
    return false;
}

void Widget::set_mouse_events_enabled(bool enabled) {
    set_attribute(ATTR_MOUSE_EVENTS_ENABLED, enabled);
}

Point Widget::map_to_local(Point pos) const {
    ncpp::NotCurses& nc = ncpp::NotCurses::get_instance();
    nc.get_stdplane()->translate(plane, &pos.y, &pos.x);
    return pos;
}

Point Widget::map_to_global(Point pos) const {
    ncpp::NotCurses& nc = ncpp::NotCurses::get_instance();
    plane->translate(nc.get_stdplane(), &pos.y, &pos.x);
    return pos;
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


Widget* Widget::get_widget_under_mouse() {
    if (Widget* wid = get_focused_widget(); wid) {
        Widget* tl = wid->get_top_level_widget();
        return tl->get_deepest_widget_under_mouse();
    }
    return nullptr;
}

void Widget::set_mouse_position(Point pos) {
    mouse_position = pos;
}

Point Widget::get_mouse_position() {
    return mouse_position;
}

// void Widget::focus_next_child() {
//     if (layout) {
//         int num_childs = layout->get_children().size();
//         int tries = 0;
//
//         while(tries < num_childs) {
//             focused_child_idx++;
//
//             Widget* child = layout->widget_at(focused_child_idx);
//             if (child && child->get_focus_policy() == FocusPolicy::FOCUS) {
//                 child->set_focus();
//                 break;
//             }
//
//             if (focused_child_idx >= layout->get_children().size()) {
//                 focused_child_idx = -1;
//             }
//             tries++;
//         }
//     }
// }
//
// void Widget::focus_prev_child() {
// }

Widget* Widget::focus_next_in_chain() {

    Widget* first = next_in_focus_chain();
    if (!first) {
        return nullptr;
    }

    Widget* wid = nullptr;
    do {
        wid = next_in_focus_chain();
        curr_focus_node = curr_focus_node->next;
    }
    while (wid->get_focus_policy() != FocusPolicy::FOCUS || wid != first);

    wid->set_focus();
    return wid;
}

Widget* Widget::focus_prev_in_chain() {

    Widget* first = prev_in_focus_chain();
    if (!first) {
        return nullptr;
    }

    Widget* wid = nullptr;
    do {
        wid = prev_in_focus_chain();
        curr_focus_node = curr_focus_node->prev;
    }
    while (wid->get_focus_policy() != FocusPolicy::FOCUS && wid != first);

    wid->set_focus();
    return wid;
}

void Widget::delete_focus_graph() {
    if (focus_graph) {
        FocusNode* curr = focus_graph;
        do {
            FocusNode* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        while(curr != focus_graph);
    }
    focus_graph = nullptr;
    curr_focus_node = nullptr;
}


void Widget::set_tab_order(Widget* wid, Widget* next) {
    if (!focus_graph) {
        FocusNode* node = new FocusNode;
        node->widget = wid;
        node->next = node;
        node->prev = node;

        focus_graph = node;
        curr_focus_node = node;
    }

    FocusNode* node = focus_graph;

    // TODO: guard infinite loop if wid not in graph
    while(node->widget != wid) {
        node = node->next;
    }

    // Set next of wid to next
    node->next = new FocusNode;
    node->next->widget = next;

    // Set prev of next to wid
    node->next->prev = node;

    // Set the next of next to the first
    node->next->next = focus_graph;

    // Set the prev of the first to the last
    focus_graph->prev = node->next;
}

Widget* Widget::next_in_focus_chain() {
    if (curr_focus_node) {
        return curr_focus_node->next->widget;
    }
    return nullptr;
}

Widget* Widget::prev_in_focus_chain() {
    if (curr_focus_node) {
        return curr_focus_node->prev->widget;
    }
    return nullptr;
}

void Widget::use_focus_stack() {
    focus_stack = new FocusStack;
    focus_stack->push(this);
}

FocusStack* Widget::get_next_parent_focus_stack() {
    Widget* prnt = parent;
    while(prnt && !prnt->focus_stack) {
        prnt = prnt->parent;
    }

    return prnt ? prnt->focus_stack : nullptr;
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
                w->dirty = false;
            }
            else if (Layout* b = item->get_layout(); b){
                // TODO: draw any widgets inside of a layout that does not
                // have its own widget
            }
        }
    }
}

void Widget::set_dirty() {
    dirty = true;
    if (layout) {
        for (LayoutItem* child : layout->get_children()) {
            if (Widget* wid = child->get_widget(); wid) {
                wid->set_dirty();
            }
        }
    }
}

FocusStack* Widget::get_focus_stack() {
    return focus_stack;
}

}
