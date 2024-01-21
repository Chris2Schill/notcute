#pragma once

#include <boost/signals2.hpp>
#include <notcurses/notcurses.h>
#include "object.hpp"
#include "box.hpp"
#include "rect.hpp"
#include "colors.hpp"

namespace notcute {


template<typename T>
using signal = boost::signals2::signal<T>;

template<typename T>
using slot = boost::signals2::slot<T>;

class Layout;
class Event;
class DrawEvent;
class KeyboardEvent;
class ResizeEvent;

enum class FocusPolicy {
    FOCUS,
    NO_FOCUS
};

// Fills all cells in a plane with cell c
void fill(ncpp::Plane* plane, const ncpp::Cell& c);

// Fills all cells in a plane with string c
void fill(ncpp::Plane* plane, const std::string& c);

// Draws the coords relative to the widget in the
// widgest frame
void draw_coords(Widget* w);

class Widget : public Object {
public:
    Widget(Widget* parent = nullptr);
    ~Widget() {
        log_debug(fmt::format("deleting {} widget", get_name()));
        delete box;

        log_debug(fmt::format("deleting {}s ncplane", get_name()));
        delete plane;
    }

    // Starts a render/event loop for this widget.
    // If a ncpp::Pile has not been created for this
    // widget yet, one will be created. Only one
    // ncpp::Pile can be rendered to the screen at a time
    // so calling show on a widget 
    void show();

    // Will end the render/event loop for this widget
    void done_showing();

    // Sets the layout of the widget.
    virtual void set_layout(Box* layout);

    // Note for pre_draw and draw()
    // For all internal uses, the widgets own plane is passed as
    // the param, but it takes a ncpp::Plane* as param to allow
    // for a widget to draw its contents to anothers
    // for some fancyschmance drawing in application code

    // Positions and resizes the owned ncpp::Plane according
    // to the box layout model.
    // Recursively pre_draws all child widget's planes.
    // Used internally and should not need to be called
    // directly for most use cases.
    virtual void pre_draw(ncpp::Plane* plane);

    // Does the actual draws to the plane. Recursively
    // draws all child widgets plans as well.
    // for some fancyschmance drawing
    virtual void draw(ncpp::Plane* plane);

    // Schedules a redraw event for this widget that will
    // happen in the next pump of the eventloop  
    virtual void redraw();

    // Scheduls a redraw for the top level widget of the view tree,
    // effectively redrawing all widget
    // TODO: This seems like a crutch. reevaluate the need for this.
    void redraw_all();

    // Gets the associated ncpp::Plane for this widget.
    // Every widget has a plane.
    ncpp::Plane* get_plane();

    // Returns the widget's layout if one exists and
    // nullptr otherwise
    Box* get_layout();

    // Gets the parent widget, will be nullptr for a top_level
    // widget
    Widget* get_parent();

    // Reparent this widget's plane and all planes bound to
    // this widget's plane to the plane owned by 'new_parent'.
    // This is used internally and most application code
    // should not need to call this directly
    void reparent(Widget* new_parent);

    // Event handling
    bool on_event_start(Event* e);
    virtual bool on_event(Event* e);
    virtual bool on_draw_event(DrawEvent* e);
    virtual bool on_keyboard_event(KeyboardEvent* e);
    virtual bool on_resize_event(Event* e);

    // Virtual getter for a string representation of a widget
    virtual std::string to_string() const;

    // FocusPolicy::FOCUS means the widget can take
    // input focus events.
    // FocusPolicy::NOFOCUS means it will ignore any attempts
    // for it to take focus
    void set_focus_policy(FocusPolicy fp);
    FocusPolicy get_focus_policy() const;

    // Attempts to set the widget as the "focused_widget".
    // see Widget::get_focused_widget()
    void set_focus();

    // Returns if this widget is the "focused_widget"
    bool is_focused();

    // Updates the layout geometry. Remember that if a widget's
    // layout has set_behave() of LAY_FILL of any kind, then
    // changes here will get overwritten by the box model
    virtual void set_geometry(const Rect& rect);
    Rect get_geometry();

    // A widget can be top level if is created without a parent,
    // or if it was take()n from its owning layout.
    bool is_top_level() const;

    // Traverses the up view tree to find the top level 
    // widget (one that does not have a parent and thus
    // probably has an associated ncpp::Pile if is the one
    // beign shown)
    Widget* get_top_level_widget();

    // A convenience take function so applicationc ode
    // does not have to specify a parent. Only use if you are
    // 100% sure the widget has a parent and is in a layout or
    // else it will assert false
    Widget* take();

    // Schedules the widget to be deleted at a later time.
    // Only use if you are 100% sure the widget does not have
    // a parent and is not in a layout
    void delete_later();

    // Convenience function for the common(?)
    // take(); delete_later();
    // Maybe I'll remove this
    void take_and_delete_later();

    // Helper methods to set the planes foreground/background
    // color. TODO: verify that ncpp::Plane::set_fg_rgb
    // is working correctly for channels with alpha bits set.
    // (I don't think it is, and is why I use ncpp::Plane::set_channels()
    // but I would like these to work)
    void set_fg_color(Color c);
    void set_bg_color(Color c);

    // Currently theres no internal usage of a widgets name
    // other than for debugging
    void set_name(const std::string& name);
    const std::string& get_name() const;

    // Used for debugging. Maybe this should be changed
    // to set_base_cell, but I am trying to limit the
    // exposure of the Notcurses api.
    void debug_set_plane_color(int r, int g, int b);

    // Signals
    signal<void(bool)> focus_state_changed;

    // Only one widget can have focus at a time, i.e
    // the "focused_widget". Focus heirarchies that
    // provide control flow between widgets can be done
    // using the FocusStack with one or more FocusNode graphs
    static Widget*& get_focused_widget();

    static ncplane_options& default_options();

protected:

    // Draws the widgts children. Used in draw()
    // to recursively draw the view tree
    void draw_children();

    ncpp::Plane* plane = nullptr;


private:
    Box*         box = nullptr;
    Widget*      parent = nullptr;
    bool         is_showing = false; //not to be confused with a visible widget
    FocusPolicy  focus_policy = FocusPolicy::NO_FOCUS;
    bool         dirty = false;
    static std::unordered_map<ncpp::Plane*, std::string> plane_name_map;
    static Widget* focused_widget;
};

inline void fill(ncpp::Plane* plane, const ncpp::Cell& c) {
    plane->home();
    Rect rect {
        Point{
            .x = plane->get_x(),
            .y = plane->get_y(),
        },
        Size {
            .width = plane->get_dim_x(),
            .height = plane->get_dim_y(),
        }
    };

    for (int i = 0; i < rect.height(); i++) {
        for (int j = 0; j < rect.width(); j++) {
            plane->putc(i, j, c);
        }
    }
}

inline void fill(ncpp::Plane* plane, const std::string& c) {
    plane->erase();

    Rect rect {
        Point{
            .x = plane->get_x(),
            .y = plane->get_y(),
        },
        Size {
            .width = plane->get_dim_x(),
            .height = plane->get_dim_y(),
        }
    };

    for (int i = 0; i < rect.height(); i++) {
        for (int j = 0; j < rect.width(); j++) {
            plane->putstr(i, j, c.c_str());
        }
    }
}

inline void draw_coords(Widget* w) {
    Rect rect = w->get_layout()->get_rect();
    w->get_plane()->putstr(0,0, fmt::format("({},{})", rect.y(), rect.x()).c_str());

    if (Widget* p = w->get_parent(); p) {
        p->get_plane()->translate_abs(&rect.m_pos.y, &rect.m_pos.x);
        std::string str = fmt::format("({},{})", rect.y(), rect.x());
        w->get_plane()->putstr(0,rect.width()-str.size(), str.c_str());
    }
}


}
