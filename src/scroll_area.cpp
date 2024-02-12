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

ScrollArea::~ScrollArea() {
    if (content) {
        delete content;
    }
}


void ScrollArea::draw(ncpp::Plane* p) {
    if (!content) {
        log_debug("SCROLL AREA CONTENT NULL");
        return;
    }

    // Not exactly sure if this is the best place for this but..
    // when taking/adding the scroll area from a layout, its possible
    // that the 'content' can end up above the scroll areas visible plane
    // on the z-axis. So force it below (again) here..
    ncplane_move_family_bottom(content->get_plane()->to_ncplane());

    // Draws the content area plane. content should be an 
    // "offscreen" widget.
    content->get_layout()->run_context();
    content->pre_draw(content->get_plane());
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

bool ScrollArea::on_mouse_event(MouseEvent* e) {
    switch(e->get_button()) {
        case NCKEY_BUTTON4: // Scrollwheel up
            scroll_vertically(-1);
            return true;
        case NCKEY_BUTTON5: // Scrollwheel down
            scroll_vertically(1);
            return true;
    }
    return FrameWidget::on_mouse_event(e);
}

}
