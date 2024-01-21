#pragma once

#include "widget.hpp"

namespace notcute {

struct WidgetGraphNode {
    WidgetGraphNode* next;
    WidgetGraphNode* prev;

    notcute::Widget* widget;
};

}
