#pragma once

#include "rect.hpp"
#include "layout2.hpp"

namespace notcute {

class Box {
public:
    Box(int rows, int cols, Box* parent = nullptr) : Box(parent) {
        set_size(rows, cols);
    }

    Box(Box* parent = nullptr) {
        ctx = notcute::get_layout_context();
        lay_reserve_items_capacity(ctx, 1024);
        id = lay_item(ctx);

        if (parent) {
            lay_insert(ctx, parent->id, id);
        }
    }

    void set_size(int rows, int cols) {
        lay_set_size_xy(ctx, id, cols, rows);
    }

    void set_contain(int flags) {
        lay_set_contain(ctx, id, flags);
    }

    void set_behave(int flags) {
        lay_set_behave(ctx, id, flags);
    }

    void insert(Box* box) {
        lay_insert(ctx, id, box->id);
    }

    void run_context(){
        lay_run_context(ctx);
    }

    Rect get_rect() {
        lay_vec4 rect = lay_get_rect(ctx, id);
        return Rect {
            Point{rect[0],rect[1]},
            Size{(unsigned)rect[2],(unsigned)rect[3]},
        };
    }

    lay_id       id = {};
    lay_context* ctx = {};
    int          box_flags = 0;
    int          layout_flags = 0;
};

}
