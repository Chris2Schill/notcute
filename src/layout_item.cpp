#include <notcute/layout_item.hpp>

#include <notcute/widget.hpp>

namespace notcute {

WidgetItem::~WidgetItem() {
    delete wid;
}

void WidgetItem::release_ownership() {
    wid = nullptr;
}

Layout* WidgetItem::get_layout() {
    return wid->get_layout();
}

}

