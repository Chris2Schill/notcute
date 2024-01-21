#include "notcute/glyphs.hpp"
#include "notcute/logger.hpp"
#include <notcute/scroll_area.hpp>

#include <notcute/text_widget.hpp>

namespace notcute{

// TODO: Why does setting ncpp::Plane::set_scrolling(true) on a widget in the
// 'content' widget cause a crash? Does set_scrolling invalidate the original plane?

void ScrollArea::draw(ncpp::Plane* p) {
    if (!content) {
        log_debug("SCROLL AREA CONTENT NULL");
        return;
    }

    // Not excatly sure if this is the best place for this but..
    // when taking/adding the scroll area from a layout, its possible
    // that the 'content' can end up above the scroll areas visible plane
    // on the z-axis. So force it below (again) here..
    content->get_plane()->move_bottom();

    // Draws the content area plane. content should be an 
    // "offscreen" widget.
    draw_content(content);

    // Preserve then channels that are already on the plane
    uint64_t chans = p->get_channels();

    // Renders the 'content' widget (and all of its children)
    // to a singular plane containing the contents of
    // the 'content' widget as if it had been rendered to the screen
    ncpp::Plane* pflattened = create_flat_merged_plane(content);

    // Copy a subsection of 'pflattened' to the actually visible
    // scroll area plane 'p'. Doing this on a per cell basis
    // because that was the only way I knew to capture both the glyphs
    // and the ncchannel of each cell individually.
    // Note: After coding this I discovered ncplane_render_buffer
    // which may be a more efficient/easier way of doing this.
    Rect r = get_geometry();
    int rows = r.rows();
    int cols = r.cols();
    // 1 -> n-1 due to forced borders of scroll area
    for (int y = 1; y < rows-1; ++y) {
        for (int x = 1; x < cols-1; ++x) {
            ncpp::Cell cell;
            pflattened->get_at(content_subwindow.y+(y-1),
                               content_subwindow.x+(x-1),
                               cell);

            p->set_channels(cell.get_channels());
            p->putc(y,x,cell);
        }
    }
    delete pflattened;

    // Restore the planes channels
    p->set_channels(chans);

    FrameWidget::draw(p);
}

// Recursively traverses the view tree to create a singular
// plane containing the contents of all the Widget 'w's
// plane and its childrens planes
ncpp::Plane* ScrollArea::create_flat_merged_plane(Widget* w) {

    ncpp::Plane* p = new ncpp::Plane(*w->get_plane());
    p->move(w->get_plane()->get_y(), w->get_plane()->get_x());

    for (LayoutItem* box : w->get_layout()->get_children()) {
        if (Widget* cw = box->get_widget(); cw) {
            ncpp::Plane* cp = create_flat_merged_plane(cw); 
            cp->mergedown_simple(p); 
            delete cp;
        }
    }

    return p;
}

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

}
