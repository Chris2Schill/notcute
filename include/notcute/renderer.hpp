#pragma once

#include <fcntl.h>
#include <unordered_map>
#include <ncpp/Pile.hh>
#include "widget.hpp"

namespace notcute {

int resize_cb(ncplane* p);

struct Renderer {
    static ncplane_options* default_pile_options() {
        auto& nc = ncpp::NotCurses::get_instance();
        unsigned rows, cols;
        nc.get_term_dim(&rows, &cols);

        static ncplane_options opts = {
            // .rows = rows,
            // .cols = cols,
            .flags = NCPLANE_OPTION_MARGINALIZED
        };

        return &opts;
    }


    void show(Widget* widget) {
        create_pile_for_widget(widget);
        piles[widget]->show();
    }

    static Renderer* get_instance() {
        return instance;
    }

    ncpp::Pile* get_pile(Widget* w) {
        return piles[w];
    }

    static notcute::Point get_term_center() {
        auto size = Renderer::get_instance()->get_term_size();
        return Point{
            .x = static_cast<int>(size.width/2),
            .y = static_cast<int>(size.height/2),
        };
    }

    ncpp::Pile* create_pile_for_widget(Widget* w) {
        auto iter = piles.find(w);
        if (iter == piles.end()) {
            auto pile = new ncpp::Pile(default_pile_options(), &nc);
            pile->set_resizecb(&resize_cb);
            piles[w] = pile;
        }
        return piles[w];
    }

    Size get_term_size() {
        auto& nc = ncpp::NotCurses::get_instance();
        unsigned rows, cols;
        nc.get_term_dim(&rows, &cols);
        return Size{.width=cols, .height=rows};
    }

    ncpp::NotCurses* get_nc() { return &nc; }

    void resize_all_piles() {
        log_debug("RENDERER REDRAW ALL PILES");
        Size term_size = get_term_size();
        for (auto& [wid, pile] : piles) {
            Rect r = wid->get_geometry();
            r.set_width(term_size.width);
            r.set_height(term_size.height);
            wid->set_geometry(r);
            wid->get_layout()->rebuild_layout();
            // wid->get_plane()->resize(pile->get_dim_y(), pile->get_dim_x());
            // wid->get_plane()->resize_realign();
            wid->redraw();
            log_debug(fmt::format("RENDERER: h,w=({},{})", term_size.height, term_size.width));
        }
    }

private:
    static Renderer* instance;
    std::unordered_map<Widget*, ncpp::Pile*> piles;
    ncpp::NotCurses nc;
};

inline int resize_cb(ncplane* p) {
    Renderer* renderer = Renderer::get_instance();
    // log_debug(fmt::format("RENDERER: resize_cb h,w=({},{})", ncplane_dim_y(p), ncplane_dim_x(p)));
    renderer->resize_all_piles();
    return 0;
}

}
