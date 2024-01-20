#pragma once

#include <notcurses/notcurses.h>
#include <string>

#include "widget.hpp"
#include "event_loop.hpp"
#include "scroll_area.hpp"
#include "main_window.hpp"

namespace notcute {

class ListItem {
public:
    virtual ~ListItem() = default;

    virtual Widget* get_widget() { return nullptr; }

    virtual std::string to_string() const = 0;
};


template<typename T>
std::string to_string(const T&);

template<typename T>
class ListItem_t : public ListItem {
public:
    ListItem_t(T t) : obj(t) {}

    virtual std::string to_string() const override {
        return notcute::to_string<T>(obj);
    }

    T& get_item() {
        return obj;
    }

private:
    T obj;
};

template<typename T>
T list_widget_item_t_get_item(notcute::ListItem* item) {
    // Will throw an exception if the dynamic cast fails
    ListItem_t<T>& obj = dynamic_cast<ListItem_t<T>&>(*item);
    return obj.get_item();
}

class ListTextItem : public ListItem {
public:
    ListTextItem(const std::string& text)  
        : text(text)
    {
    }

    virtual std::string to_string() const override { return text; }

private:
    std::string text;
};

class ListWidgetItem : public ListItem {
public:
    ListWidgetItem(Widget* w) : wid(w) {}
    ~ListWidgetItem() { delete wid; }


    virtual Widget* get_widget() override { return wid; }

    std::string to_string() const override {
        return wid->to_string();
    }

private:
    Widget* wid = nullptr;
};

class ListWidget : public ScrollArea {
public:
    ListWidget(Widget* parent = nullptr)
        :ScrollArea(nullptr, parent)
    {
        set_layout(new VBoxLayout);
        set_focus_policy(FocusPolicy::FOCUS);

        uint32_t fg, bg;
        fg = NCCHANNEL_INITIALIZER(1,1,1);
        bg = NCCHANNEL_INITIALIZER(0,0,0);
        bg &= NCALPHA_TRANSPARENT;

        // plane->set_channels(ncchannels_combine(fg, bg));

        Widget* list_content = new Widget(this);
        // TODO: this size should not have to be set 
        list_content->set_layout(new VBoxLayout(200,200));
        set_content(list_content);
    }
    ~ListWidget() {
        delete_items();
    }

    // void set_geometry(const Rect& r) override {
    //     ScrollArea::set_geometry(r);
    // }

    // void redraw() override {
    //     ScrollArea::redraw();
    // }

    void add_item(ListItem* item) {
        bool prev = is_selected_idx_valid();
        items.push_back(item);
        bool valid = is_selected_idx_valid();

        if (valid != prev && valid) {
            item_hovered(items[selected_idx]);
        }
        redraw();
        content->redraw();
        resize_content();
    }

    void resize_content() {
        // Rect cr = get_geometry();
        // cr.set_height(items.size());
        // content->set_geometry(cr);
        // content->get_layout()->run_context();

        if (should_adjust_to_contents) {
            adjust_size_to_contents();
        }
    }

    void add_text(const std::string& text) {
        add_item(new ListTextItem(text));
    }

    void add_widget(Widget* w) {
        w->reparent(this);
        add_item(new ListWidgetItem(w));
    }

    void delete_items() {
        for (ListItem* item : items) {
            delete item;
        }
        items.clear();
        redraw();
    }

    void clear() {
        delete_items();
        redraw();
    }

    void next_item() {
        if (selected_idx < items.size()-1) {
            selected_idx++;
            if (selected_y_offset == get_visible_height()) {
                selected_y_offset++;
            }
            else {
                scroll_vertically();
            }
            notcute::log_debug("listwidget nextitem");
            // if (selected_idx > get_visible_height()+content_subwindow.y-1) {
            //     scroll_vertically();
            // }
            redraw();
        }
    }

    void prev_item() {
        if (selected_idx != 0) {
            selected_idx--;
            if (selected_idx < content_subwindow.y) {
                scroll_vertically(-1);
            }
            redraw();
        }
    }

    void set_should_adjust_size_to_contents(bool adjust) {
        should_adjust_to_contents = adjust;

        resize_content();
    }

    void adjust_size_to_contents() {
        int width = 0;
        int height = items.size();

        for (auto& item : items) {
            int w = item->to_string().size();
            if (w > width) { width = 0; }
        }
        content->get_plane()->resize(height, width);
        redraw();
    }

    bool is_selected_idx_valid() const {
        return (0 <= selected_idx && selected_idx < items.size());
    }

    void draw_content(Widget* content) override {
        ncpp::Plane* cp = content->get_plane();

        for (int i = 0; i < items.size(); ++i)
        {
            ListItem* item = items[i];
            if (!item) { continue; }
            std::string str = item->to_string();
            cp->putstr(i, 0, str.c_str());
            bool selected = i == selected_idx;
            highlight_row(i, selected);
        }
        redraw();
    }

    void highlight_row(int y, bool selected) {
        Color fg = selected ? fg_selected : fg_notselected;
        Color bg = selected ? bg_selected : bg_notselected;

        ncpp::Plane* cp = content->get_plane();
        for (int col = 0; col < cp->get_dim_x(); ++col) {
            ncpp::Cell c;
            cp->get_at(y,col,c);
            c.set_fg_rgb8(fg.red, fg.green, fg.blue);
            c.set_bg_rgb8(bg.red, bg.green, bg.blue);
            cp->putc(y,col,c);
        }
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch (e->get_key()) {
            case 'j':
            case NCKEY_DOWN:
                next_item();
                if (is_selected_idx_valid()) {
                    item_hovered(items[selected_idx]);
                }
                return true;
            case 'k':
            case NCKEY_UP:
                prev_item();
                if (is_selected_idx_valid()) {
                    item_hovered(items[selected_idx]);
                }
                return true;
            case NCKEY_ENTER:
                if (is_selected_idx_valid()) {
                    item_selected(items[selected_idx]);
                }
                redraw();
                return true;
            default:
                return ScrollArea::on_keyboard_event(e);
        }
    }

    void draw(ncpp::Plane* plane) override {
        ScrollArea::draw(plane);

        int selected = is_selected_idx_valid() ? selected_idx + 1 : 0;
        std::string curr_item = fmt::format("{} of {}", selected, items.size());
        plane->putstr(plane->get_dim_y()-1, plane->get_dim_x()-2-curr_item.size(), curr_item.c_str());
    }

    const std::vector<ListItem*>& get_items() { return items; }

    int get_selected_idx() const { return selected_idx; }

    // Signals
    boost::signals2::signal<void(ListItem*)> item_hovered;
    boost::signals2::signal<void(ListItem*)> item_selected;

    void set_fg_selected(Color c)    { fg_selected = c; }
    void set_bg_selected(Color c)    { bg_selected = c; }
    void set_fg_notselected(Color c) { fg_notselected = c; }
    void set_bg_notselected(Color c) { bg_notselected = c; }

private:
    std::vector<ListItem*> items;
    int selected_idx = 0;
    bool should_adjust_to_contents = false;
    notcute::Color bg_selected    = {200,200,255};
    notcute::Color fg_selected    = {0, 000, 0};
    notcute::Color bg_notselected = {0,0,0};
    notcute::Color fg_notselected = {200,0,0};

    int selected_y_offset = 0;
};

}
