#include "notcute/glyphs.hpp"
#include "notcute/logger.hpp"
#include <notcurses/notcurses.h>
#include <notcute/scroll_area.hpp>

#include <notcute/text_widget.hpp>

namespace notcute{

// TODO: Why does setting ncpp::Plane::set_scrolling(true) on a widget in the
// 'content' widget cause a crash? Does set_scrolling invalidate the original plane?

// We don't want the up/down arrows to be consider part of the height calculations
static int ROWS_NOT_PART_OF_VISIBLE_HEIGHT = 2;
static int COLS_NOT_PART_OF_VISIBLE_WIDTH = 2;


ScrollArea::ScrollArea(Widget* content, Widget* parent)
    :FrameWidget(parent)
    ,content(content)
{
    if (content) {
        set_content(content);
    }
    set_focus_policy(FocusPolicy::FOCUS);
}

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

void ScrollArea::set_content(Widget* w) { 
    content = w;

    // Move the content plane to the bottom of the pile
    // even below top level widgets plane. The content
    // widget and its children are rendered "offscreen"
    // and then a specific subsection of that plane is
    // copy-pastad onto the scroll areas plane. This is
    // how we achieve the "window" effect where the plane
    // of the scroll area seems to act as a "window" to the content
    // thats "behind" it. The content plane is not actually moved
    // around behind the scrollarea plane as we scroll.
    ncplane* np = content->get_plane()->to_ncplane();
    ncplane_move_family_bottom(np);

    redraw();
}

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

void ScrollArea::draw_content(Widget* content) {
    content->get_layout()->run_context();
    content->pre_draw(content->get_plane());
    content->draw(content->get_plane());
}

void ScrollArea::draw_border(ncpp::Plane* p) {
    assert(p == get_plane());

    FrameWidget::draw_border(p);
    auto chans = p->get_channels();

    set_fg_color({255,255,255,NCALPHA_OPAQUE});
    set_bg_color(get_frame_bg());
    draw_vertical_scrollbar(p);

    p->set_channels(chans);
}

void ScrollArea::draw_vertical_scrollbar(ncpp::Plane* p) {
    if (!is_content_height_fully_visible()) {
        // float pct_of_content_shown = get_pct_of_content_shown();
        // notcute::log_debug(fmt::format("SCROLLAREA pct_of_content_shown={}", pct_of_content_shown));

        int scrollbar_rows = get_scrollbar_row_count();
        // notcute::log_debug(fmt::format("SCROLLAREA scroll_bar_rows={}", scrollbar_rows));

        int start_row = content_subwindow.y*get_pct_of_content_shown();

        // Arrows
        p->putstr(1,p->get_dim_x()-1, UP_ARROW.c_str());
        p->putstr(p->get_dim_y()-ROWS_NOT_PART_OF_VISIBLE_HEIGHT,p->get_dim_x()-1, DOWN_ARROW.c_str());

        for (int row = start_row+ROWS_NOT_PART_OF_VISIBLE_HEIGHT; row < scrollbar_rows+start_row; ++row) {
            p->putstr(row,p->get_dim_x()-1, FULL_VERTICAL_BLOCK.c_str());
        }
    }
}

int ScrollArea::get_visible_height() {
    return get_plane()->get_dim_y() - ROWS_NOT_PART_OF_VISIBLE_HEIGHT;
}

int ScrollArea::get_visible_width() {
    return get_plane()->get_dim_x() - COLS_NOT_PART_OF_VISIBLE_WIDTH;
}

int ScrollArea::get_content_height() {
    if (content) {
        return content->get_plane()->get_dim_y();
    }
    return 0;
}

int ScrollArea::get_content_width() {
    if (content) {
        return content->get_plane()->get_dim_x();
    }
    return 0;
}

int ScrollArea::get_scrollbar_row_count() {
    return static_cast<int>(get_visible_height()*get_pct_of_content_shown());
}

float ScrollArea::get_pct_of_content_shown() {
    float vis_height = static_cast<float>(get_visible_height());
    float content_height = static_cast<float>(get_content_height());
    return vis_height/content_height;
}

bool ScrollArea::is_content_height_fully_visible() {
    return get_content_height() <= get_visible_height();
}

bool ScrollArea::is_content_width_fully_visible() {
    return get_content_width() <= get_visible_width();
}
void ScrollArea::adjust_subwindow_y(int& coord, int adjustment) {
    coord += adjustment; 
    // coord = std::clamp(coord, 0, (int)(get_scrollbar_row_count()*(1.0/get_pct_of_content_shown()))+ROWS_NOT_PART_OF_VISIBLE_HEIGHT);
    coord = std::clamp(coord, 0, get_content_height()-get_visible_height());
    redraw();
}

void ScrollArea::adjust_subwindow_x(int& coord, int adjustment) {
    coord += adjustment; 
    coord = std::clamp(coord, 0, (int)(content->get_plane()->get_dim_x() - get_visible_width()));
    redraw();
}

void ScrollArea::scroll_vertically(int rows) {
    if (!is_content_height_fully_visible()) {
        adjust_subwindow_y(content_subwindow.y, rows);
        scrolled();
    }
}

void ScrollArea::scroll_horizontally(int cols) {
    if (!is_content_width_fully_visible()) {
        adjust_subwindow_x(content_subwindow.x, cols);
        scrolled();
    }
}

bool ScrollArea::on_keyboard_event(KeyboardEvent* e) {
    switch (e->get_key()) {
        case 'h':
        case NCKEY_LEFT:
            scroll_horizontally(-1);
            return true;
        case 'l':
        case NCKEY_RIGHT:
            scroll_horizontally(1);
            return true;
        case 'j':
        case NCKEY_DOWN:
            scroll_vertically(1);
            return true;
        case 'k':
        case NCKEY_UP:
            scroll_vertically(-1);
            return true;
        default:
            return FrameWidget::on_keyboard_event(e);
    }
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

}
