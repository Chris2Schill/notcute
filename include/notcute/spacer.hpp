#pragma once

#include "widget.hpp"
#include "box.hpp"

namespace notcute {

class Spacer : public Widget {
public:
    Spacer(Widget* parent = nullptr) : Widget(parent) {
        set_layout(new Box);
        get_layout()->set_behave(LAY_FILL);
    }
};


}
