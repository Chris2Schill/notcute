#pragma once

#include "widget.hpp"

namespace notcute {

class Dialog : public Widget {
public:
    Dialog(Widget* parent = nullptr)
        : Widget(parent)
    {
    }


};


}
