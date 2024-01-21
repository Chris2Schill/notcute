#pragma once

#include <ncpp/NotCurses.hh>

namespace notcute {

struct Color {
    int red   = 0;
    int green = 0;
    int blue  = 0;
    unsigned alpha = NCALPHA_OPAQUE;
    uint32_t to_channel() {
        uint32_t ch = NCCHANNEL_INITIALIZER(red, green, blue);
        ncchannel_set_alpha(&ch, alpha);
        return ch;
    }
};

inline uint64_t channels_from_fgbg(notcute::Color fg, notcute::Color bg) {
    uint32_t fgc = fg.to_channel();
    uint32_t bgc = bg.to_channel();
    return (static_cast<uint64_t>(fgc)<<32) + bgc;
}

inline uint64_t RGB(int r, int g, int b) {
    ncpp::Cell c(' ');
    c.set_fg_rgb8(r,g,b);
    return c.get_channels();
}

inline static notcute::Color WHITE = {235,235,235,NCALPHA_OPAQUE};
inline static notcute::Color DARK_YELLOW = {180,120,000,NCALPHA_OPAQUE};

}
