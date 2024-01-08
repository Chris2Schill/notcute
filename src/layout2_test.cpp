// LAY_IMPLEMENTATION needs to be defined in exactly one .c or .cpp file that
// includes layout.h. All other files should not define it.

#include "notcute/layout2.h"
#define LAY_IMPLEMENTATION
#include <notcute/layout2.hpp>
#include <notcute/renderer.hpp>
#include <notcute/logger.hpp>
#include <iostream>

using namespace notcute;

void print(std::string name, lay_vec4 vec) {
    log_debug(fmt::format("{} = Pos=({},{}),Size=({},{})", name, vec[0], vec[1], vec[2], vec[3]));
    // std::cout << fmt::format("{} = Pos=({},{}),Size=({},{})\n", name, vec[0], vec[1], vec[2], vec[3]);
};
void print(std::string name, Rect rect) {
    // log_debug(fmt::format("{} = Pos=({},{}),Size=({},{})", name, vec[0], vec[1], vec[2], vec[3]));
    log_debug(fmt::format("{} = Pos=({},{}),Size=({},{})", name, rect.x(), rect.y(), rect.width(), rect.height()));
};

lay_context* get_context() {
    static lay_context ctx;
    static bool first = true;
    if (first) {
        first = false;
        lay_init_context(&ctx);
    }
    return &ctx;
}

class Box {
public:
    Box(int rows, int cols, Box* parent = nullptr) : Box(parent) {
        set_size(rows, cols);
    }

    Box(Box* parent = nullptr) {
        ctx = get_context();
        lay_reserve_items_capacity(ctx, 1024);
        id = lay_item(ctx);

        if (parent) {
            lay_insert(ctx, parent->id, id);
        }
    }

    // Box(int width, int height) {
    //
    //     ctx = get_context();
    //     lay_reserve_items_capacity(ctx, 1024);
    //     id = lay_item(ctx);
    //     lay_set_size_xy(ctx, id, width, height);
    // }

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

ncpp::Plane* from_box(Box* box) {
    Rect r = box->get_rect();
    return new ncpp::Plane(r.height(), r.width(), r.y(), r.x());
}

void fill(ncpp::Plane* plane, std::string c) {
    plane->erase();

    Rect rect {
        Point{
            .x = plane->get_x(),
            .y = plane->get_y(),
        },
        Size {
            .width = plane->get_dim_x(),
            .height = plane->get_dim_y(),
        }
    };

    for (int i = 0; i < rect.height(); i++) {
        for (int j = 0; j < rect.width(); j++) {
            plane->putstr(i, j, c.c_str());
        }
    }
}

void draw(Box* b, ncpp::Plane* p, const std::string& c) {
    Rect r = b->get_rect();
    p->resize(r.height(), r.width());
    p->move(r.y(), r.x());
    // p->resize(100,100);
    // p->move(0,0);

    fill(p, c);
    p->perimeter_double(0,0,0);
    p->putstr(0,2,"TOPLEFTBABY");
}

struct Widget {
    Box* bounds;
    ncpp::Plane* plane;
};

int main() {
    // notcute::Renderer* renderer = notcute::Renderer::get_instance();
    // Size term_size = renderer->get_term_size();

    ncpp::NotCurses nc;
    unsigned rows, cols;
    nc.get_term_dim(&rows, &cols);
    Box* root2 = new Box(rows, cols);
    // Box* root2 = new Box(1280,800);
    root2->set_contain(LAY_ROW);

    Box* mlist = new Box(20, 60, root2);
    Box* mcontents = new Box(1,1, root2);

    Box* mlist_i1 = new Box(20, 20, mlist);
    mlist_i1->set_behave(LAY_FILL);
    Box* mlist_i2 = new Box(20, 20, mlist);
    mlist_i2->set_behave(LAY_FILL);
    Box* mlist_i3 = new Box(20, 20, mlist);
    mlist_i3->set_behave(LAY_FILL);


    mlist->set_behave(LAY_VCENTER | LAY_LEFT | LAY_VFILL);
    mlist->set_contain(LAY_COLUMN);
    //
    // root2->insert(mcontents);
    mcontents->set_behave(LAY_HFILL | LAY_VFILL);

    root2->run_context();


    print("root", root2->get_rect());
    print("master list", mlist->get_rect());
    print("contents", mcontents->get_rect());
    // print("contents", mcontents->get_rect());


    ncpp::Plane* mplane = from_box(mlist);
    ncpp::Plane* cplane = from_box(mcontents);
    ncpp::Plane* mlist_i1p = from_box(mlist_i1);
    ncpp::Plane* mlist_i2p = from_box(mlist_i2);
    ncpp::Plane* mlist_i3p = from_box(mlist_i3);

    draw(mlist, mplane, "m");
    draw(mcontents, cplane, "z");
    draw(mlist_i1, mlist_i1p, "a");
    draw(mlist_i2, mlist_i2p, "b");
    draw(mlist_i3, mlist_i3p, "c");

    while(true) {
        nc.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    // Rect crect = mcontents->get_rect();
    // print("contents2", crect);
    return 0;


    // notcute::Renderer* renderer = notcute::Renderer::get_instance();
    //
    //
    // Widget* main_window = new Widget();
    // main_window->set_layout(new BoxLayout);
    // main_window->set_name("main_window");
    //
    //
    // Widget* row1 = new Widget(main_window);
    // row1->set_fill("a");
    // row1->set_layout(new BoxLayout);
    //
    // Widget* row2 = new Widget(main_window);
    // row2->set_fill("a");
    // row2->set_layout(new BoxLayout);
    //
    // main_window->get_layout()->add_widget(row1);
    //
    // main_window->show();
    auto& ctx = *notcute::get_layout_context();

    // And we need to initialize it
    lay_init_context(&ctx);

    // Let's pretend we're creating some kind of GUI with a master list on the
    // left, and the content view on the right.


    // The context will automatically resize its heap buffer to grow as needed
    // during use. But we can avoid multiple reallocations by reserving as much
    // space as we'll need up-front. Don't worry, lay_init_context doesn't do any
    // allocations, so this is our first and only alloc.
    lay_reserve_items_capacity(&ctx, 1024);

    // Create our root item. Items are just 2D boxes.
    lay_id root = lay_item(&ctx);

    // Let's pretend we have a window in our game or OS of some known dimension.
    // We'll want to explicitly set our root item to be that size.
    lay_set_size_xy(&ctx, root, 1280, 720);

    // Set our root item to arrange its children in a row, left-to-right, in the
    // order they are inserted.
    lay_set_contain(&ctx, root, LAY_ROW);

    // Create the item for our master list.
    lay_id master_list = lay_item(&ctx);
    lay_insert(&ctx, root, master_list);
    // Our master list has a specific fixed width, but we want it to fill all
    // available vertical space.
    lay_set_size_xy(&ctx, master_list, 400, 0);
    // We set our item's behavior within its parent to desire filling up available
    // vertical space.
    lay_set_behave(&ctx, master_list, LAY_VFILL);
    // And we set it so that it will lay out its children in a column,
    // top-to-bottom, in the order they are inserted.
    lay_set_contain(&ctx, master_list, LAY_COLUMN);

    lay_id content_view = lay_item(&ctx);
    lay_insert(&ctx, root, content_view);
    // The content view just wants to fill up all of the remaining space, so we
    // don't need to set any size on it.
    //
    // We could just set LAY_FILL here instead of bitwise-or'ing LAY_HFILL and
    // LAY_VFILL, but I want to demonstrate that this is how you combine flags.
    lay_set_behave(&ctx, content_view, LAY_HFILL | LAY_VFILL);

    // Normally at this point, we would probably want to create items for our
    // master list and our content view and insert them. This is just a dumb fake
    // example, so let's move on to finishing up.

    // Run the context -- this does all of the actual calculations.
    lay_run_context(&ctx);

    // Now we can get the calculated size of our items as 2D rectangles. The four
    // components of the vector represent x and y of the top left corner, and then
    // the width and height.
    lay_vec4 master_list_rect = lay_get_rect(&ctx, master_list);
    lay_vec4 content_view_rect = lay_get_rect(&ctx, content_view);

    print("master list", master_list_rect);
    print("content view", content_view_rect);
    
    // master_list_rect  == {  0, 0, 400, 720}
    // content_view_rect == {400, 0, 880, 720}

    // If we're using an immediate-mode graphics library, we could draw our boxes
    // with it now.
    // my_ui_library_draw_box_x_y_width_height(
    //     master_list_rect[0],
    //     master_list_rect[1],
    //     master_list_rect[2],
    //     master_list_rect[3]);

    // You could also recursively go through the entire item hierarchy using
    // lay_first_child and lay_next_sibling, or something like that.

    // After you've used lay_run_context, the results should remain valid unless a
    // reallocation occurs.
    //
    // However, while it's true that you could manually update the existing items
    // in the context by using lay_set_size{_xy}, and then calling lay_run_context
    // again, you might want to consider just rebuilding everything from scratch
    // every frame. This is a lot easier to program than tedious fine-grained
    // invalidation, and a context with thousands of items will probably still only
    // take a handful of microseconds.
    //
    // There's no way to remove items -- once you create them and insert them,
    // that's it. If we want to reset our context so that we can rebuild our layout
    // tree from scratch, we use lay_reset_context:

    lay_reset_context(&ctx);

    // And now we could start over with creating the root item, inserting more
    // items, etc. The reason we don't create a new context from scratch is that we
    // want to reuse the buffer that was already allocated.

    // But let's pretend we're shutting down our program -- we need to destroy our
    // context.
    lay_destroy_context(&ctx);

    // The heap-allocated buffer is now freed. The context is now invalid for use
    // until lay_init_context is called on it again.
}
