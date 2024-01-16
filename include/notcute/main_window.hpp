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
        Widget::draw(p);
        if (draw_line_numbers) {
            Rect r = get_geometry();
            for (int i = 0; i < r.height(); i++) {
                plane->putstr(i, 0, std::to_string(i).c_str());
            }
        }
    }

    void set_draw_line_numbers() {
        draw_line_numbers = true;
    }
private:
    bool draw_line_numbers = false;
};

}
