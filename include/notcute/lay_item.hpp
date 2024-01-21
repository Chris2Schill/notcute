#pragma once

#include "layout.h"
#include "logger.hpp"

namespace notcute {

// Lay_Item is a simple C++ wrapper class for the box model provided by the layout.h
// library.
class Lay_Item {
public:
    Lay_Item(int rows, int cols) {
        ctx =  new lay_context;
        lay_init_context(ctx);
        lay_reserve_items_capacity(ctx, 1024);
        id = lay_item(ctx);
        set_size(rows, cols);
        log_debug(fmt::format("LAY_ITEM created top level lay_item: id = {}", id));
    }
    Lay_Item(Lay_Item& parent) {
        ctx = parent.ctx;
        id = lay_item(ctx);
        log_debug(fmt::format("LAY_ITEM created sub level lay_item: id = {}", id));
    }

    ~Lay_Item() {
        // 0 id means its the root of the view tree
        log_debug(fmt::format("LAY_ITEM deleting lay_item: id = {}", id));
        if (id == 0) {
            log_debug(fmt::format("LAY_ITEM destroyed context: id = {}", id));
            lay_destroy_context(ctx);
            delete ctx;
        }
    }

    // Wrapper functions to the c interface
    void set_size(int rows, int cols) { lay_set_size_xy(ctx, id, cols, rows); }
    void set_contain(int flags) { lay_set_contain(ctx, id, flags); }
    void set_behave(int flags) { lay_set_behave(ctx, id, flags); }
    void insert(Lay_Item* item) { lay_insert(ctx, id, item->id); }
    void set_margins_ltrb(lay_scalar left, lay_scalar top, lay_scalar right, lay_scalar bottom) { lay_set_margins_ltrb(ctx,id,left, top, right, bottom); }
    void get_margins_ltrb(lay_scalar* left, lay_scalar* top, lay_scalar* right, lay_scalar* bottom) { lay_get_margins_ltrb(ctx,id,left,top,right, bottom); }
    void run_context() { lay_run_context(ctx); }
    void reset_context() { lay_reset_context(ctx); }
    void run_item() { lay_run_item(ctx, id); }
    lay_context* get_context() { return ctx; }
    lay_vec4 get_rect() { return lay_get_rect(ctx, id); }
    std::function<void(Lay_Item*)> resize_callback;

private:
    lay_id       id = {};
    lay_context* ctx = nullptr;
};

}
