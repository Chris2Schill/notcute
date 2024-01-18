#include "notcute/logger.hpp"
#include <notcute/scroll_area.hpp>

#include <notcute/text_widget.hpp>

namespace notcute{

void ScrollArea::draw_children(Widget* w) {

    ncpp::Plane* p = get_plane();
    int left_margin, top_margin, _;
    content->get_layout()->get_margins_ltrb(&left_margin, &top_margin, &_, &_);

    int yoffset = content->get_geometry().y() - top_margin;
    int xoffset = content->get_geometry().x() - left_margin;

    p->translate_abs(&yoffset,&xoffset);

    for (BoxItem* box : w->get_layout()->get_children()) {
        if (Widget* cw = box->get_widget(); cw) {
            ncpp::Plane* cp = cw->get_plane();

            Rect cprect = cw->get_geometry();
            p->translate_abs(&cprect.m_pos.y, &cprect.m_pos.x);

            int cursory = cprect.y()-yoffset;
            int cursorx = cprect.x()+xoffset;

            cursory -= content_subwindow.y;
            cursorx -= content_subwindow.x;

            p->cursor_move(cursory,cursorx);
            for (int y = 0; y < cp->get_dim_y(); ++y) {
                for (int x = 0; x < cp->get_dim_x(); ++x) {
                    ncpp::Cell cell;
                    cp->get_at(y,x,cell);

                    p->set_channels(cell.get_channels());
                    p->putc(cursory+y,cursorx+x,cell);
                }
            }
            draw_children(cw);
        }
    }

}

void ScrollArea::draw(ncpp::Plane* p) {

    Widget::draw_children();
    // // p->set_scrolling(true);
    //
    // draw_children(content);
    // return;

    log_debug("FLAT DRAW TEXTWIDGET---------------------");
    if (!content) {
        return;
    }
    content->pre_draw(content->get_plane());
    content->draw(content->get_plane());
    ncpp::Plane* pflattened = create_flat_merged_plane(content);
    pflattened->move(0, 30);


    // int y = 1;
    // int x = 1;
    // pflattened->translate(stdplane, &y, &x);
    // pflattened->move(0,-10);
    //

    // ncpp::Plane* cpy = new ncpp::Plane(*pflattened);
    // cpy->move(content->get_plane()->get_y(), content->get_plane()->get_x());

    content->get_plane()->erase();
    int rows = content->get_plane()->get_dim_y();
    int cols = content->get_plane()->get_dim_y();
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            ncpp::Cell cell;
            pflattened->get_at(y+content_subwindow.y,
                               x+content_subwindow.x,
                               cell);

            // cell.set_bg_rgb8(0,0,0);
            p->set_channels(cell.get_channels());
            p->putc(y,x,cell);
        }
    }
    // delete pflattened;
}

ncpp::Plane* ScrollArea::create_flat_merged_plane(Widget* w) {

    ncpp::Plane* stdplane = ncpp::NotCurses::get_instance().get_stdplane();

    ncpp::Plane* p = new ncpp::Plane(*w->get_plane());
    p->move(w->get_plane()->get_y(), w->get_plane()->get_x());
    // p->reparent(w->get_plane()->get_parent());

    Rect wrect = w->get_geometry();
    notcute::log_debug(fmt::format("FLAT {}, (w,h) = ({},{}), (y,x)=({},{})",
                w->get_name(), wrect.width(), wrect.height(), wrect.y(), wrect.x()));

    const auto& children = w->get_layout()->get_children();

    int y = w->get_plane()->get_y();
    int x = w->get_plane()->get_x();
    // int y = 0;//w->get_plane()->get_y();
    // int x = 0;//w->get_plane()->get_x();
    int by = y;
    int bx = x;

    int yabs,xabs;
    ncplane_abs_yx(w->get_plane()->to_ncplane(), &yabs, &xabs);
    // w->get_plane()->translate(p->get_parent(), &y,&x);

    // p->translate(content->get_plane(), &yabs,&xabs);
    // content->get_plane()->translate(p, &yabs,&xabs);

    // p->translate(content->get_plane(), &y,&x);
    // w->get_plane()->translate(p, &y,&x);

    notcute::log_debug(fmt::format("TEXTWIDGET {}, BEFORE=({},{}), AFTER=({},{}) ABS=({},{})", 
                w->get_name(), by, bx, y, x, yabs, xabs));

    // ncplane_move_yx(stdplane->to_ncplane(), y, x);
    // p->move(yabs, xabs);
    // p->move_top();
    // p->reparent_family(w->get_plane());

    // First resize the dupped plane to account for all the children
    // ACCOUNTING FOR VERTICAL ONLY
    // if (children.size() > 0) {
    //     Widget* first  = children.front()->get_widget();
    //     Widget* last  = children.back()->get_widget();
    //
    //     if (first && last) {
    //         Rect r = last->get_geometry();
    //         int width = wrect.right()-wrect.left();
    //         int height = r.bottom()-wrect.top();
    //         p->resize(
    //                 r.bottom() - wrect.top(),
    //                 wrect.right() - wrect.left()
    //                 );
    //         notcute::log_debug(fmt::format("FLAT {}, (w,h) = ({},{}), (y,x)=({},{})",
    //                     w->get_name(), width, height, wrect.y(), wrect.x()));
    //     }
    //     else {
    //         log_debug("ERROR WITH SCROLL AREA FLATTEN");
    //     }
    // }
    // else
    // {
    //     notcute::log_debug(fmt::format("FLAT NO RESIZE {}, (w,h) = ({},{}), (y,x)=({},{})", 
    //                 w->get_name(), wrect.width(), wrect.height(), wrect.y(), wrect.x()));
    // }
    // p->resize_realign();


    for (BoxItem* box : w->get_layout()->get_children()) {
        if (Widget* cw = box->get_widget(); cw) {
            ncpp::Plane* cp = create_flat_merged_plane(cw); 
            // cp->move_above(p);


            // int fy = cp->get_y();
            // int fx = cp->get_x();
            // cp->get_parent()->translate_abs(&fy,&fx);
            //
            // notcute::log_debug(fmt::format("FLAT {}, BEFORE=({},{}), AFTER=({},{})", 
            //             w->get_name(), cp->get_y(), cp->get_x(), fy, fx));
            //
            // cp->move(fy, fx);

            cp->mergedown_simple(p); 

            delete cp;
        }
    }

    
    // if (w->get_parent())
    {
    }


    return p;
}

ncpp::Plane* create_flat_merged_plane_recursive_helper(Widget* p) {

    return nullptr;
}


void ScrollArea::draw3(ncpp::Plane* p) {
    // content->get_plane()->putstr("DARPPPP");
    // char* cp_content = content->get_plane()->content(0,0,5,5);
    Widget::draw_children();
    // cp2->erase();
    p->set_scrolling(true);
    // p->putstr(cp_content);

    int left_margin, top_margin, _;
    content->get_layout()->get_margins_ltrb(&left_margin, &top_margin, &_, &_);

    for (BoxItem* box : content->get_layout()->get_children()) {
        if (Widget* w = box->get_widget(); w) {
            ncpp::Plane* cp = w->get_plane();
            // char* cp_content = cp->content(0,0,1,1);
            char* cp_content = cp->content(0,0,
                                           cp->get_dim_y(), cp->get_dim_x());
            if (cp_content) {

                Rect cprect = w->get_geometry();
                p->translate_abs(&cprect.m_pos.y, &cprect.m_pos.x);

                int yoffset = content->get_geometry().y() - top_margin;
                int xoffset = content->get_geometry().x() - left_margin;
                // cp->translate_abs(&yoffset,&xoffset);
                p->translate_abs(&yoffset,&xoffset);

                //
                // yoffset -= 1;//top_margin;
                // xoffset += 1;//left_margin;

                // int yoffset = -cprect.y();
                // int xoffset = -cprect.x();;

                // log_debug("cprect.height = " + std::to_string(cprect.height()));
                // log_debug("yoffset= = " + std::to_string(yoffset));

                int cursory = cprect.y()-yoffset;
                int cursorx = cprect.x()+xoffset;

                cursory -= content_subwindow.y;
                cursorx -= content_subwindow.x;

                p->cursor_move(cursory,cursorx);
                p->putstr(cp_content);
                // int xcurs = p->cursor_x();
                // p->putstr(p->cursor_y(), p->cursor_x(), );
            }
            else {
                log_debug("Failed to draw scroll area child");
            }
        }
    }
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