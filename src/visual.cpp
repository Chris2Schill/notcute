#include <notcute/visual.hpp>

#include <ncpp/Visual.hh>

namespace notcute {

Visual* Visual::from_file(const std::string& file, Widget* parent) {
    auto mp = new Visual(parent);
    mp->set_layout(new VBoxLayout);
    mp->visual =  new ncpp::Visual(file.c_str());
    mp->vopts = {
        .n = mp->get_plane()->to_ncplane(),
        .scaling = NCSCALE_STRETCH,
        .blitter = NCBLIT_BRAILLE,
        .flags = NCVISUAL_OPTION_NOINTERPOLATE,
    };
    return mp;
}

void Visual::draw(ncpp::Plane* p) {
    if (is_playing) {
        visual->decode_loop();
    }
    visual->blit(&vopts);
}

void Visual::post_draw(ncpp::Plane*) {
    if (is_playing) {
        redraw();
    }
}

void Visual::set_is_playing(bool playing) {
    is_playing = playing;
    if (is_playing) {
        redraw();
    }
}

bool Visual::get_is_playing() const {
    return is_playing;
}

void Visual::set_blitter(ncblitter_e b) {
    vopts.blitter = b;
    redraw();
}

}
