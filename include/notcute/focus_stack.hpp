#pragma once

#include <stack>
#include <notcute/widget.hpp>

namespace notcute {

// Describes a focus tree that is used to create a widgets focus chain.
// It is a double circularly linked-list
struct FocusNode {
    notcute::Widget* widget;
    FocusNode* next;
    FocusNode* prev;
};

// Delete the entire focus chain that 'node' is a part of
void delete_focus_chain(FocusNode* node);

class FocusStack {
public:

    void push(Widget* wid) {
        assert(wid->get_focus_policy() == FocusPolicy::FOCUS);
        the_stack.push(wid);
        wid->set_focus();
    }

    // Will pop the top widget off the stack without
    // changing the focus state
    Widget* pop() {
        assert(the_stack.size() > 1);
        Widget* top = the_stack.top();
        the_stack.pop();
        return top;
    }

    // Will pop the top widget and focus the new top widget.
    // Returns the widget that was popped off the stack
    Widget* pop_focus() {
        assert(the_stack.size() > 1);

        Widget* popped = the_stack.top();
        the_stack.pop();

        Widget* top = the_stack.top();
        assert(top->get_focus_policy() == FocusPolicy::FOCUS);
        top->set_focus();

        return popped;
    }

    // Will clear the focus stack, while keeping the owning
    // Widget as the only remaining element. The 'owning'
    // widget is the first widget ever pushed onto the stack
    void clear() {
        while(the_stack.size() > 1) {
            the_stack.pop();
        }
    }

    std::stack<Widget*> the_stack;
};

// Access to a static stack of Widgets for more complex
// focus control
std::stack<Widget*>& get_focus_stack();

// Push/pop operations on the focus stack
// that will call set_focus on widgets appropriately 
void focus_stack_push(Widget* wid);
Widget* focus_stack_pop();

}
