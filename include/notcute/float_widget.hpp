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

    void move_to_term_center() {
        assert(get_layout() != nullptr);

        Rect rect = get_geometry();
        Point term_center = Renderer::get_term_center();

        term_center.x -= rect.width()/2;
        term_center.y -= rect.height()/2;

        rect.set_pos(term_center);
        set_geometry(Rect{term_center, rect.size()});
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

    void draw(ncpp::Plane* p) override {
        draw_solid_bg(p);
    }

    void draw_solid_bg(ncpp::Plane* p, uint64_t channels = 0) {
        notcute::fill(p, " ");
        p->perimeter_rounded(0,channels,0);
    }

private:
    Point pos;
};

}
