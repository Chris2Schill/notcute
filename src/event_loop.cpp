#include <notcute/event_loop.hpp>
#include <notcute/widget.hpp>

namespace notcute {

void EventLoop::process_events() {

    while (true) {
        auto [event, err] = events.read();
        if (err) { break; }

        if (event->get_sender()) {
            event->get_sender()->on_event(event);
        }
        else if(event->get_type() == Event::KEYBOARD_EVENT ||
                event->get_type() == Event::MOUSE_EVENT) {
            if (Widget::get_focused_widget()) {
                Widget::get_focused_widget()->on_event(event);
            }
        }
        else {
            for (auto& sub : subscribers) {
                sub.callback(event);
            }
        }
        delete event;
    }
}

}
