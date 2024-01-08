
#define LAY_IMPLEMENTATION
#include <notcute/layout.hpp>

namespace notcute {

lay_context* get_layout_context() {
    static lay_context ctx;
    static bool first = true;
    if (first) {
        first = false;
        lay_init_context(&ctx);
        lay_reserve_items_capacity(&ctx, 1024);
    }
    return &ctx;
}


}
