#pragma once

#include "widget.hpp"
#include "renderer.hpp"

namespace notcute {

class FloatWidget : public Widget {
public:
    FloatWidget(Widget* parent = nullptr)
        :Widget(parent)
    {
        get_plane()->move_top();
    }
    ~FloatWidget()
    {
        get_top_level_widget()->redraw();
    }

    void move(Point pos) {
        assert(get_layout() != nullptr);

        // Point p = map_to_local(pos);

        Rect rect = get_geometry();
        rect.set_pos(pos);
        set_geometry(rect);
    }

    void move(int y, int x) {
        move(Point{
                .x = x,
                .y = y,
            });
    }

    void move_to_term_center() {
        Rect r = get_geometry();
        Point term_center = Renderer::get_term_center();
        term_center.x -= r.width()/2;
        term_center.y -= r.height()/2;
        move(Point{.x = term_center.x,
                   .y = term_center.y});
    }

    virtual void pre_draw(ncpp::Plane* p) override {
        Widget::pre_draw(p);
        p->move(pos.y, pos.x);
        p->resize_realign();
    }

    void set_geometry(const Rect& r) override {
        Widget::set_geometry(r);
        pos = r.pos();
        redraw();
    }

    void set_pos(Point p) {
        Rect rect = get_geometry();
        rect.set_pos(p);
        set_geometry(rect);
    }

private:
    Point pos;
};

}
