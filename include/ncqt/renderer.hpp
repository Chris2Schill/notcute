#pragma once

#include <fcntl.h>
#include <unordered_map>
#include <ncpp/Pile.hh>
#include "widget.hpp"

namespace ncqt {

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
        static Renderer* instance = new Renderer;
        return instance;
    }

    ncpp::Pile* get_pile(Widget* w) {
        return piles[w];
    }

    ncpp::Pile* create_pile_for_widget(Widget* w) {
        auto iter = piles.find(w);
        if (iter == piles.end()) {
            auto pile = new ncpp::Pile(default_pile_options(), &nc);
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

private:
    std::unordered_map<Widget*, ncpp::Pile*> piles;
    ncpp::NotCurses nc;
};

}
