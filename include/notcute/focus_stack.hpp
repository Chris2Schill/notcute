#pragma once

#include <vector>
#include <functional>
#include <stack>
#include <notcute/widget.hpp>

namespace notcute {

// Describes a focus tree that can be used to link widget's focus
struct FocusNode {
    notcute::Widget*                widget;  // a widget that can receive "focus"
    void*                           userptr; // anything extra you want
    std::function<void(FocusNode*)> on_focus_callback; // called when node gains focus
    std::function<void(FocusNode*)> off_focus_callback;// called when node loses focus

    FocusNode* next;     // the next node
    FocusNode* prev;     // the prev node
    FocusNode* selected; // can be used to point to any other node, can even be a different focus graph

    void off_focus() { if (off_focus_callback){ off_focus_callback(this);} }
    void on_focus()  { if (on_focus_callback) { on_focus_callback(this); } }

    FocusNode* focus_next() {
        off_focus();
        next->on_focus();
        widget->redraw();
        return next;
    }
};

// a helper container to make construction of graph easier
struct FocusNodeEntry {
    notcute::Widget* widget; 
    void*            userptr; 
};

std::vector<FocusNode*> setup_focus_graph(const std::vector<FocusNodeEntry>& focusables,
                                          std::function<void(FocusNode*)> on_focus_callback = nullptr,
                                          std::function<void(FocusNode*)> off_focus_callback = nullptr);

struct FocusStackEntry {
    // the widget that orchestrates the focus graph. This is the widget
    // that actuall has focus, it will delegate input events to
    // nodes in the tree as it sees fit
    notcute::Widget* orchestrator;

    // pointer to the focus node that had "focus" at this snapshot in time
    FocusNode**      active_node;
};

std::stack<FocusStackEntry>& get_focus_stack();
void focus_stack_push(FocusStackEntry n);
FocusStackEntry focus_stack_pop();
void delete_focus_graph(FocusNode* node);

}
