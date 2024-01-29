#pragma once
#include <signal.h>

#include "widget.hpp"
#include "renderer.hpp"

void sig_handler(int sig);

namespace notcute {

class MainWindow;

static MainWindow* mw_instance = nullptr;

class MainWindow : public Widget {
public:
    MainWindow() {
        mw_instance = this;

        notcute::Renderer* renderer = notcute::Renderer::get_instance();
        Size term_size = renderer->get_term_size();
        new VBoxLayout(term_size.height, term_size.width, this);

        ::signal(SIGINT, sig_handler);

        set_focus_policy(FocusPolicy::FOCUS);
        set_focus();
        set_name("Main Window");


        // Why does redrawing here cause a blank screen
        // if we have another top level widget created
        // before showing this widget?
        // redraw();
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


inline void sig_handler(int sig) {
    notcute::log_debug("ctrl-c pressed");
    notcute::mw_instance->done_showing();

}
