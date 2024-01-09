#pragma once

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
        set_layout(new VBoxLayout(parent));
        set_focus_policy(FocusPolicy::FOCUS);
    }

    void add_item(ListItem* item) {
        items.push_back(item);
    }

    void add_text(const std::string& text) {
        add_item(new ListTextItem(text));
    }

    void add_widget(Widget* w) {
        w->reparent(this);
        get_layout()->insert(w->get_layout());
        add_item(new ListWidgetItem(w));
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
    }

    void on_keyboard_event(KeyboardEvent* e) override {
        switch (e->get_key()) {
            case 'j':
            case NCKEY_DOWN:
                next_item();
                break;
            case 'k':
            case NCKEY_UP:
                prev_item();
                break;
            case NCKEY_ENTER:
                item_selected(items[selected_idx]);
                break;
            default:
                break;
        }
    }

    void draw(ncpp::Plane* plane) override {
        // fill(plane, "z");
        // plane->putstr(9,0, "TEST");
        // return;
        int i = 0;
        for (int row = row_start; row < items.size() && i <= get_layout()->get_rect().height(); ++i, ++row) {
            ListItem*& item = items[row];

            plane->set_channels(NCCHANNEL_INITIALIZER(1,1,1));
            if (row == selected_idx) {
                plane->set_bg_palindex(2);
                plane->set_fg_palindex(2);
            }
            else {
                plane->set_bg_palindex(1);
                plane->set_fg_palindex(1);
            }
            plane->putstr(i,0, item->to_string().c_str());
        }
        plane->putstr(i+1,0, "TESSTTTT");

        // plane->perimeter_rounded(0, 0, 0);
        // draw_coords(this);
    }

    const std::vector<ListItem*>& get_items() { return items; }

    // Signals
    boost::signals2::signal<void(ListItem*)> item_selected;

private:
    std::vector<ListItem*> items;
    int selected_idx = 0;
    int row_start = 0;

};

}
