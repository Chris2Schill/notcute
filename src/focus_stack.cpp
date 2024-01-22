#include <notcute/focus_stack.hpp>

namespace notcute {

std::vector<FocusNode*> setup_focus_graph(const std::vector<FocusNodeEntry>& focusables,
                                          std::function<void(FocusNode*)> on_focus_callback,
                                          std::function<void(FocusNode*)> off_focus_callback)
{
    // Create focus nodes for each focusable
    std::vector<FocusNode*> nodes;
    for (auto focusable : focusables) {
        FocusNode* node = new FocusNode{
            .widget = focusable.widget,
            .userptr = focusable.userptr,
            .on_focus_callback = on_focus_callback,
            .off_focus_callback = off_focus_callback,
        };
        nodes.push_back(node);

        // Force any configuration via callbacks to take effect immediately
        node->on_focus();
        node->off_focus();
    }

    // link prev/next nodes 
    FocusNode* first = nodes.front();
    first->prev = nodes.back();
    first->next = *(++nodes.begin());
    FocusNode* last = nodes.back();
    last->next = nodes.front();
    last->prev = *(--(--nodes.end()));
    for (int i = 1; i < nodes.size()-1; ++i) {
        FocusNode* node = nodes[i];
        node->next = nodes[i+1];
        node->prev = nodes[i-1];
    }

    return nodes;
}

std::stack<FocusStackEntry>& get_focus_stack() {
    static std::stack<FocusStackEntry> nodes;
    return nodes;
}

void focus_stack_push(FocusStackEntry n) {
    assert(n.orchestrator->get_focus_policy() == notcute::FocusPolicy::FOCUS);

    if (get_focus_stack().size() > 0) {
        FocusStackEntry top = get_focus_stack().top();
        FocusNode* node = *top.active_node;
        node->off_focus();
    }

    get_focus_stack().push(n);
    n.orchestrator->set_focus();
    FocusNode* node = *n.active_node;
    node->on_focus();
}

FocusStackEntry focus_stack_pop() {
    auto& fs = get_focus_stack();
    assert(fs.size() > 0);
    FocusStackEntry popped = fs.top();
    fs.pop();
    FocusStackEntry top = fs.top();
    assert(top.orchestrator->get_focus_policy() == notcute::FocusPolicy::FOCUS);
    top.orchestrator->set_focus();

    FocusNode* popped_node = *popped.active_node;
    FocusNode* top_node    = *top.active_node;

    popped_node->off_focus();
    top_node->on_focus();

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
