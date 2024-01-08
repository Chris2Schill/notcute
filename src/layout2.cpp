
#define LAY_IMPLEMENTATION
#include <notcute/layout2.hpp>

namespace notcute {

lay_context* get_layout_context() {
    static lay_context ctx;
    static bool first = true;
    if (first) {
        first = false;
        lay_init_context(&ctx);
    }
    return &ctx;
}


}
