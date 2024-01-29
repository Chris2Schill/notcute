#include "notcute/widget.hpp"
#include <notcute/focus_stack.hpp>

namespace notcute {

std::stack<Widget*>& get_focus_stack() {
    static std::stack<Widget*> widgets;
    return widgets;
}

void focus_stack_push(Widget* n) {
    assert(n->get_focus_policy() == FocusPolicy::FOCUS);
    get_focus_stack().push(n);
    n->set_focus();
}

Widget* focus_stack_pop() {
    auto& fs = get_focus_stack();
    assert(fs.size() > 0);

    Widget* popped = fs.top();
    fs.pop();

    Widget* top = fs.top();
    assert(top->get_focus_policy() == notcute::FocusPolicy::FOCUS);
    top->set_focus();

    return popped;
}

void delete_focus_graph(FocusNode* node) {
    FocusNode* curr = node;
    do {
        FocusNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    while(curr != node);
}

}
