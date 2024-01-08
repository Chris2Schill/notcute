#include <notcute/layout2.hpp>

namespace notcute {

lay_context* get_layout_context() {
    static lay_context ctx;
    return &ctx;
}

}
