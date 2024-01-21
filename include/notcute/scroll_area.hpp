#pragma once

#include <notcurses/notcurses.h>
#include <ncpp/Visual.hh>

#include "frame_widget.hpp"
#include "event_loop.hpp"

namespace notcute {

class ScrollArea : public FrameWidget {
public:
    ScrollArea(Widget* content, Widget* parent = nullptr);
    ~ScrollArea() = default;

    void draw(ncpp::Plane* p) override;

    // Sets the widget it to be viewed in the scroll area
    void set_content(Widget* w);

    // Recursively traverses the view tree to create a singular
    // plane containing the contents of all the Widget 'w's
    // plane and its childrens planes
    ncpp::Plane* create_flat_merged_plane(Widget* p);

    // Virtual draw specifically for the 'content'
    virtual void draw_content(Widget* content);

    void draw_border(ncpp::Plane* p) override;
    void draw_vertical_scrollbar(ncpp::Plane* p);

    int get_visible_height();
    int get_visible_width();
    int get_content_height();
    int get_content_width();
    int get_scrollbar_row_count();
    float get_pct_of_content_shown();

    bool is_content_height_fully_visible();
    bool is_content_width_fully_visible();

    void adjust_subwindow_y(int& coord, int adjustment);
    void adjust_subwindow_x(int& coord, int adjustment);

    void scroll_vertically(int rows = 1);
    void scroll_horizontally(int cols = 1);
    
    bool on_keyboard_event(KeyboardEvent* e) override;

    // Signals
    notcute::signal<void()> scrolled;

protected:
    Widget* content;
    struct {
        int y = 0;
        int x = 0;
    } content_subwindow;;


private:
    int row_start = 0;
};

}
