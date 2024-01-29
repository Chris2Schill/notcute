#pragma once

#include "notcute/box_layout.hpp"
#include "widget.hpp"

namespace notcute {

// Stacked widget will only show one of its children at a time
class StackedWidget : public Widget {
public:
    StackedWidget(Widget* parent = nullptr)
        :Widget(parent)
    {
        set_layout(new VBoxLayout);
    }

    bool set_index(int i) {
        if (is_child_idx_valid(i)) {
            if (is_child_idx_valid(child_idx)) {
                get_layout()->take(children[child_idx]);
            }
            child_idx = i;
            get_layout()->add_widget(children[child_idx]);
            redraw();
            return true;
        }
        return false;
    }

    void add_widget(Widget* child) {
        children.push_back(child);
        child->reparent(this);
        if (!is_child_idx_valid(child_idx)) {
            child_idx = 0;
            get_layout()->add_widget(child);
        }
        else {
            child->reparent(nullptr);
        }
        redraw();
    }

    void next_child() {
        int next = (child_idx+1) % children.size();
        set_index(next);
    }

    void prev_child() {
        int next = (child_idx-1+children.size()) % children.size();
        set_index(next);
    }

    int get_index() const { return child_idx; }

    int get_num_children() const { return children.size(); }

private:
    bool is_child_idx_valid(int idx) {
        return 0 <= idx && idx < children.size();
    }

    std::vector<Widget*> children;
    int child_idx = -1;
};

}
