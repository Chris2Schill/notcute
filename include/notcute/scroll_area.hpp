#pragma once

#include <notcurses/notcurses.h>
#include <ncpp/Visual.hh>
#include <string>

#include "frame_widget.hpp"
#include "event_loop.hpp"
#include "glyphs.hpp"

namespace notcute {

class ScrollArea : public FrameWidget {
public:
    ScrollArea(Widget* content, Widget* parent = nullptr)
        :FrameWidget(parent)
        ,content(content)
    {
        // get_layout()->add_widget(content);
        if (content) {
            set_content(content);
        }
        get_plane()->set_bg_alpha(NCALPHA_OPAQUE);
        set_focus_policy(FocusPolicy::FOCUS);
    }
    ~ScrollArea() {
    }

    void draw(ncpp::Plane* p) override;
    void draw2(ncpp::Plane* p);

    ncpp::Plane* create_flat_merged_plane(Widget* p);
    // void draw_children(Widget* w);


    void set_content(Widget* w) { 
        content = w;

        // content->get_layout()->set_margins_ltrb(1,1,1,1);
        // content->reparent(nullptr);
        // // get_layout()->add_widget(content);
        // get_layout()->add_widget(content);
        // Rect r = get_geometry();
        // content->get_plane()->move(r.y()+1, r.x()+1);
        // content->get_layout()->set_behave(LAY_FILL);
        content->get_plane()->move_bottom();

        redraw();
        // Rect rect = get_geometry();
        // content->get_layout()->set_size(rect.rows(), rect.cols());
    }

    // We don't want the up/down arrows to be consider part of the height calculations
    int ROWS_NOT_PART_OF_VISIBLE_HEIGHT = 2;
    int COLS_NOT_PART_OF_VISIBLE_WIDTH = 2;

    int get_visible_height() {
        return get_plane()->get_dim_y() - ROWS_NOT_PART_OF_VISIBLE_HEIGHT;
    }

    int get_visible_width() {
        return get_plane()->get_dim_x() - COLS_NOT_PART_OF_VISIBLE_WIDTH;
    }

    int get_content_height() {
        if (content) {
            return content->get_plane()->get_dim_y();
        }
        return 0;
    }

    int get_content_width() {
        if (content) {
            return content->get_plane()->get_dim_x();
        }
        return 0;
    }

    int get_scrollbar_row_count() {
        return static_cast<int>(get_visible_height()*get_pct_of_content_shown());
    }

    float get_pct_of_content_shown() {
        float vis_height = static_cast<float>(get_visible_height());
        float content_height = static_cast<float>(get_content_height());
        return vis_height/content_height;
    }

    bool is_content_height_fully_visible() {
        return get_content_height() <= get_visible_height();
    }

    bool is_content_width_fully_visible() {
        return get_content_width() <= get_visible_width();
    }

    void adjust_subwindow_y(int& coord, int adjustment) {
        coord += adjustment; 
        // coord = std::clamp(coord, 0, (int)(get_scrollbar_row_count()*(1.0/get_pct_of_content_shown()))+ROWS_NOT_PART_OF_VISIBLE_HEIGHT);
        coord = std::clamp(coord, 0, get_content_height()-get_visible_height());
        redraw();
    }

    void adjust_subwindow_x(int& coord, int adjustment) {
        coord += adjustment; 
        coord = std::clamp(coord, 0, (int)(content->get_plane()->get_dim_x() - get_visible_width()));
        redraw();
    }

    void scroll_vertically(int rows = 1) {
        if (!is_content_height_fully_visible()) {
            adjust_subwindow_y(content_subwindow.y, rows);
        }
    }

    void scroll_horizontally(int cols = 1) {
        if (!is_content_width_fully_visible()) {
            adjust_subwindow_x(content_subwindow.x, cols);
        }
    }
    
    bool on_keyboard_event(KeyboardEvent* e) override {
        switch (e->get_key()) {
            case 'h':
            case NCKEY_LEFT:
                scroll_horizontally(-1);
                break;
            case 'l':
            case NCKEY_RIGHT:
                scroll_horizontally(1);
                break;
            case 'j':
            case NCKEY_DOWN:
                scroll_vertically(1);
                return true;
            case 'k':
            case NCKEY_UP:
                scroll_vertically(-1);
                return true;
            default:
                return false;
        }

        // std::clamp(content_subwindow.y, 0, get_content_height()-get_scrollbar_row_count());
        notcute::log_debug(fmt::format("SCROLL content_subwindow_yx=({},{})", content_subwindow.y, content_subwindow.x));

        return false;
    }

    // void draw(ncpp::Plane* plane) override {
    //     // int i = 0;
    //     // for (int row = row_start;
    //     //      // row < items.size() && i <= get_layout()->get_rect().height();
    //     //      i <= get_layout()->get_rect().height();
    //     //      ++i, ++row)
    //     // {
    //     //     // ListItem*& item = items[row];
    //     // }
    // }

    // Signals
    boost::signals2::signal<void(int)> scrolled;

private:
    struct {
        int y = 0;
        int x = 0;
    } content_subwindow;;

    int row_start = 0;
    Widget* content;
};

}
