#pragma once

#include "widget.hpp"
#include "renderer.hpp"

namespace notcute {

class MainWindow : public Widget {
public:
    MainWindow() {
        notcute::Renderer* renderer = notcute::Renderer::get_instance();
        Size term_size = renderer->get_term_size();
        new VBoxLayout(term_size.height, term_size.width, this);
    }

    void draw(ncpp::Plane* p) {
        // p->perimeter_rounded(0,0,0);
        // fill(p, "z");
    }
};

}
