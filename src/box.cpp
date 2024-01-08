#include <notcute/box.hpp>
#include <notcute/widget.hpp>

namespace notcute {

void Box::add_widget(Widget* widget) {
    children.push_back(new WidgetItem(widget));
    if (Box* b = widget->get_layout(); b) {
        insert(b);
    }
}

}
