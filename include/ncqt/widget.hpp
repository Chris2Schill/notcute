#pragma once

#include "object.hpp"
#include "rect.hpp"

namespace ncqt {

class Layout;
class Event;
class DrawEvent;

class Widget : public Object {
public:
    Widget(Widget* parent = nullptr);

    void    set_layout(Layout* layout);
    Layout* get_layout() const;
    void    show();

    virtual void draw(ncpp::Plane* plane) {
        plane->erase();
        draw_border();
        plane->putstr(0,0,get_name().c_str());
    }

    void reparent(Widget* new_parent) {
        // log_debug(fmt::format("{}.reparent({})", get_name(), new_parent->get_name()));
        parent = new_parent;
        plane->reparent_family(new_parent->plane);
        if (!new_parent) {
            log_debug("reparenting to null");
        }
    }

    Rect get_geometry() const {
        return Rect {
            Point{
                .x = plane->get_x(),
                .y = plane->get_y(),
            },
            Size {
                .width = plane->get_dim_x(),
                .height = plane->get_dim_y(),
            }
        };
    }

    void set_name(const std::string& name) {
        plane_name_map[plane] = name;
    }

    const std::string& get_name() {
        static const std::string NO_NAME = "(no_name";
        const std::string& name = plane_name_map[plane];
        return name.empty() ? NO_NAME : name;
    }

    ncpp::Plane* get_plane() { return plane; }

    void set_geometry(const Rect& rect);

    void draw_border() {
        plane->perimeter_rounded(0,0,0);
    }

    virtual Rect get_contents_rect() const;

    virtual void on_event(Event* e);
    virtual void on_draw_event(DrawEvent* e);

    static ncplane_options& default_options() {
        auto& nc = ncpp::NotCurses::get_instance();
        unsigned rows, cols;
        nc.get_term_dim(&rows, &cols);

        static ncplane_options opts = {
            .y = 0,
            .x = 0,
            .rows = rows,
            .cols = cols,
        };

        return opts;
    }

    Widget* get_parent() { return parent; }

protected:
    ncpp::Plane* plane = nullptr;
private:
    Layout*      layout = nullptr;
    Widget*      parent = nullptr;
    bool         is_showing = false; //not to be confused with a visible widget
    static std::unordered_map<ncpp::Plane*, std::string> plane_name_map;
};

class MainWindow : public Widget {
public:
    MainWindow();
};

}
