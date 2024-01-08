#pragma once

#include "object.hpp"
#include "box.hpp"
#include "rect.hpp"

namespace notcute {

class Layout;
class Event;
class DrawEvent;

class Widget : public Object {
public:
    Widget(Widget* parent = nullptr);

    void    set_layout(Layout* layout);
    Layout* get_layout() const;
    void    show();

    virtual void pre_draw(ncpp::Plane* plane) {
        plane->erase();
        // if (!fill_char.empty()) {
        //
        //     log_debug(fmt::format("{} DRAW {}", get_name(), get_geometry().to_string()));
        //
        //     fill(fill_char);
        //     draw_children();
        //     return;
        // }
        draw(plane);
        plane->putstr(0,0,get_name().c_str());
    }

    void draw() {
        pre_draw(plane);
        draw(plane);
        post_draw(plane);
    }

    virtual void draw(ncpp::Plane* plane) {
        plane->perimeter_rounded(0,0,0);
        plane->putstr(0, 2, "BOIII");
    }

    virtual void post_draw(ncpp::Plane* plane) {
        draw_children();
    }

    void draw_children();

    // std::string fill_char = "";
    //
    // void set_fill(std::string c) {
    //     fill_char = c;
    // }
    //
    // void fill(std::string c) {
    //     plane->erase();
    //     Rect rect = get_geometry();
    //     for (int i = 0; i < rect.height(); i++) {
    //         for (int j = 0; j < rect.width(); j++) {
    //             plane->putstr(i, j, c.c_str());
    //         }
    //     }
    // }
    //
    void reparent(Widget* new_parent) {
        parent = new_parent;
        plane->reparent_family(new_parent->plane);
    }
    //
    // Rect get_geometry() const {
    //     return Rect {
    //         Point{
    //             .x = plane->get_x(),
    //             .y = plane->get_y(),
    //         },
    //         Size {
    //             .width = plane->get_dim_x(),
    //             .height = plane->get_dim_y(),
    //         }
    //     };
    // }
    //
    // void set_name(const std::string& name) {
    //     plane_name_map[plane] = name;
    // }
    //
    const std::string& get_name() {
        static const std::string NO_NAME = "(no_name";
        const std::string& name = plane_name_map[plane];
        return name.empty() ? NO_NAME : name;
    }

    ncpp::Plane* get_plane() { return plane; }

    // void set_geometry(const Rect& rect);
    //
    // void draw_border() {
    //     plane->perimeter_rounded(0,0,0);
    // }
    //
    // virtual Rect get_contents_rect() const;
    //
    // virtual void on_event(Event* e);
    // virtual void on_draw_event(DrawEvent* e);
    //
    static ncplane_options& default_options() {
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

    Widget* get_parent() { return parent; }

protected:
    ncpp::Plane* plane = nullptr;
private:
    Box*         box = nullptr;
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
