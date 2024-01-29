#pragma once

#include "widget.hpp"

namespace ncpp {
class Visual;
}

namespace notcute {


class Visual : public Widget {
public:
    using Widget::Widget;

    static Visual* from_file(const std::string& file, Widget* parent = nullptr);

    void draw(ncpp::Plane* p) override;
    void post_draw(ncpp::Plane*) override;

    void set_is_playing(bool playing);
    bool get_is_playing() const;

    void set_blitter(ncblitter_e);

private:

    ncvisual_options vopts = {

    };
    ncpp::Visual* visual;
    bool is_playing = true;
};


}

// Keeping this old code around for when I implement notcute:Visual
#if 0
void ScrollArea::draw2(ncpp::Plane* p) {
    // draw_children();
    // return;
    Widget* mw =  get_top_level_widget();
    Widget* cp =  content;
    // Widget* cp =  get_top_level_widget();

    // unsigned begy = cp->get_plane()->get_y();
    // unsigned begx = cp->get_plane()->get_x();
    // ncvisual_options opts = {
    //     .n    = p->to_ncplane(),
    //     .y = 0,
    //     .x = 0,
    //     .begy = begy,
    //     .begx = begx,
    //     // .begy = 0,
    //     // .begx = 0,
    //     .leny = cp->get_plane()->get_dim_y(),
    //     .lenx = cp->get_plane()->get_dim_x(),
    //     // .flags = NCVISUAL_OPTION_HORALIGNED | NCVISUAL_OPTION_VERALIGNED,
    // };
    //     .flags = NCVISUAL_OPTION_CHILDPLANE
    //
    // unsigned line_size = opts.lenx*4;

    ncpp::NotCurses& nc = ncpp::NotCurses::get_instance();
    // log_debug("line_size = " + std::to_string(line_size));
    // ncvisual* ncv = ncvisual_from_rgba(rgba, opts.leny,line_size , opts.lenx);
    // if (ncv == NULL) {
    //     log_debug("failed to get ncvisual");
    //     return;
    // }

    // notcute::fill(cp->get_plane(), " ");
    // cp->get_plane()->set_bg_rgb8(0,255,255);
    // cp->get_plane()->set_fg_rgb8(0,255,255);

    cp->get_plane()->putstr(0,0, "AA23456789");
    cp->get_plane()->putstr(1,0, "BB23456789");
    cp->get_plane()->putstr(2,0, "CC23456789");
    cp->get_plane()->putstr(3,0, "DD23456789");
    cp->get_plane()->putstr(4,0, "EE23456789");
    cp->get_plane()->putstr(5,0, "FF23456789");
    cp->get_plane()->putstr(6,0, "GG23456789");
    cp->get_plane()->putstr(7,0, "HH23456789");

    // Dup Plane Test
    ncpp::Plane* cp2 = new ncpp::Plane(*cp->get_plane());
    Rect rect = cp->get_geometry();
    cp2->resize(rect.height()/2, rect.width()/2);
    cp2->move(-5,-20);
    cp2->resize_realign();

    char* cp_content = cp->get_plane()->content(1,1,5,5);
    cp2->erase();
    cp2->set_scrolling(true);
    cp2->putstr(cp_content);

    return;
    // End Dup Plane Test
    return;

    // cp->get_parent()->get_plane()->translate_abs(&rect.m_pos.y, &rect.m_pos.x);
    // This works
    static const int ALL_REMAINING_LENGTH = 0;
    // ncpp::Visual* ncv = new ncpp::Visual(
    //         *cp->get_plane(), 
    //         NCBLIT_1x1, 
    //         // cp->get_plane()->get_y(), cp->get_plane()->get_x(),
    //         0,0,
    //         ALL_REMAINING_LENGTH,
    //         ALL_REMAINING_LENGTH);


    // cp->get_plane()->home();
    uint32_t* rgba 
        = cp->get_plane()
        ->rgba(NCBLIT_1x1,
                0,
                0,
                0,
                0);
    if (!rgba) {
        log_debug("Failed to get RGBA of scroll area content");
        return;
    }
    // return;
    ncpp::Visual* ncv2 = new ncpp::Visual(
            rgba,
            cp->get_plane()->get_dim_y(),
            cp->get_plane()->get_dim_x()*4,
            cp->get_plane()->get_dim_x());

    // ncpp::Visual* ncv3 = new ncpp::Visual(
    //         "/root/erctd/3rdParty/notcurses/data/chunli15.png"
    //         );


    // explicit Visual (const Plane& p,
    // ncblitter_e blit, unsigned begy, unsigned begx, unsigned leny, unsigned lenx)

    // ncpp::Visual* ncv = new ncpp::Visual(rgba, opts.leny, 4, opts.lenx);

    // bool success = p->blit_rgba(rgba, 10, &opts);
    // int blitted = ncblit_rgba (rgba, line_size, &opts);
    struct ncvisual_options opts2 = {
        .n = get_top_level_widget()->get_plane()->to_ncplane(),
        .scaling = NCSCALE_NONE,
        // .begy = begy,
        // .begx = begx,
        // .leny = cp->get_plane()->get_dim_y(),
        // .lenx = cp->get_plane()->get_dim_x(),
        .blitter = NCBLIT_1x1,
        // .begy = 0,
        // .begx = 0,
        // .flags = NCVISUAL_OPTION_CHILDPLANE,
    };
    // ncv->blit(&opts2);
    ncv2->blit(&opts2);
    // ncv3->blit(&opts2);
    // p->blit_rgba(rgba, line_size, &opts2);
    // struct ncplane* cn;
    // if((cn = ncvisual_blit(nc, ncv, &opts2)) == NULL){
    // // if (blitted == -1) {
    //     notcute::log_debug("Blit failed");
    // }
}
#endif
