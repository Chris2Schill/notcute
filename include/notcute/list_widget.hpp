#pragma once

#include <notcurses/notcurses.h>
#include <string>

#include "widget.hpp"
#include "event_loop.hpp"

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

class ListWidget : public Widget {
public:
    ListWidget(Widget* parent = nullptr)
        :Widget(parent)
    {
        set_layout(new VBoxLayout);
        set_focus_policy(FocusPolicy::FOCUS);

        uint32_t fg, bg;
        fg = NCCHANNEL_INITIALIZER(1,1,1);
        bg = NCCHANNEL_INITIALIZER(0,0,0);
        bg &= NCALPHA_TRANSPARENT;

        plane->set_channels(ncchannels_combine(fg, bg));
    }
    ~ListWidget() {
        for (ListItem* item : items) {
            delete item;
        }
    }

    void add_item(ListItem* item) {
        bool prev = is_selected_idx_valid();
        items.push_back(item);
        bool valid = is_selected_idx_valid();

        if (valid != prev && valid) {
            item_hovered(items[selected_idx]);
        }
        redraw();
    }

    void add_text(const std::string& text) {
        add_item(new ListTextItem(text));
    }

    void add_widget(Widget* w) {
        w->reparent(this);
        add_item(new ListWidgetItem(w));
    }

    void clear() {
        for (ListItem* item : items) {
            delete item;
        }
        items.clear();
    }

    void next_item() {
        unsigned height = get_layout()->get_rect().height();
        if (selected_idx == items.size()-1) {
            selected_idx = 0;
            row_start = 0;
        }
        else {
            selected_idx++;
            if (selected_idx >= height) {
                row_start++;
                row_start = std::clamp<int>(row_start, 0, items.size()-height);
            }
        }
        redraw();
    }

    void prev_item() {
        unsigned height = get_layout()->get_rect().height();
        if (selected_idx == 0) {
            selected_idx = items.size()-1;
            if (height < items.size()) {
                row_start = items.size()-height;
            }
        }
        else {
            selected_idx--;
            if (selected_idx < row_start) {
                row_start--;
            }
        }
        redraw();
    }

    bool is_selected_idx_valid() const {
        return (0 <= selected_idx && selected_idx < items.size());
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
                return false;
        }
    }

    void draw(ncpp::Plane* plane) override {
        int i = 0;
        for (int row = row_start;
             row < items.size() && i <= get_layout()->get_rect().height();
             ++i, ++row)
        {
            ListItem*& item = items[row];

            if (row == selected_idx) {
                plane->set_bg_palindex(2);
                plane->set_fg_palindex(2);
            }
            else {
                plane->set_bg_palindex(1);
                plane->set_fg_palindex(1);
            }
            plane->putstr(i, 0, item->to_string().c_str());
        }
    }

    const std::vector<ListItem*>& get_items() { return items; }

    int get_selected_idx() const { return selected_idx; }

    // Signals
    boost::signals2::signal<void(ListItem*)> item_hovered;
    boost::signals2::signal<void(ListItem*)> item_selected;

private:
    std::vector<ListItem*> items;
    int selected_idx = 0;
    int row_start = 0;

};

}
