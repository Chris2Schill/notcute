#pragma once

#include <notcurses/notcurses.h>
#include <string>

#include "widget.hpp"
#include "list_item.hpp"
#include "scroll_area.hpp"

namespace notcute {


class ListWidget : public ScrollArea {
public:
    ListWidget(Widget* parent = nullptr);
    ~ListWidget();

    void add_item(ListItem* item);
    void add_text(const std::string& text);
    void add_widget(Widget* w);

    void clear();

    virtual void next_item();
    virtual void prev_item();

    void draw_border(ncpp::Plane* p) override;
    void draw_content(Widget* content) override;
    virtual void highlight_row(int y, bool selected);

    bool on_keyboard_event(KeyboardEvent* e) override;
    bool on_mouse_event(MouseEvent* e) override;
    void on_hover_enter() override;
    void on_hover_leave() override;

    const std::vector<ListItem*>& get_items();

    bool is_selected_idx_valid() const;
    int get_selected_idx() const;

    // Signals
    boost::signals2::signal<void(ListItem*)> item_hovered;
    boost::signals2::signal<void(ListItem*)> item_selected;

    void set_fg_selected(Color c)    { fg_selected = c; }
    void set_bg_selected(Color c)    { bg_selected = c; }
    void set_fg_notselected(Color c) { fg_notselected = c; }
    void set_bg_notselected(Color c) { bg_notselected = c; }

    struct Defaults {
        notcute::Color fg_selected    = {000,000,000, NCALPHA_OPAQUE};
        notcute::Color bg_selected    = {200,200,255, NCALPHA_OPAQUE};
        notcute::Color fg_notselected = {200,000,000, NCALPHA_OPAQUE};
        notcute::Color bg_notselected = {000,000,000, NCALPHA_TRANSPARENT};
    };
    static Defaults defaults;

private:
    void resize_content();
    void adjust_size_to_contents();
    void delete_items();
    void clicked();

    std::vector<ListItem*> items;
    int selected_idx = 0;
    notcute::Color fg_selected    = defaults.fg_selected;
    notcute::Color bg_selected    = defaults.bg_selected;
    notcute::Color fg_notselected = defaults.fg_notselected;
    notcute::Color bg_notselected = defaults.bg_notselected;
};

}
