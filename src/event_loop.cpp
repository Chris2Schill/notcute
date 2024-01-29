#include <notcute/event_loop.hpp>
#include <notcute/widget.hpp>

namespace notcute {

void EventLoop::process_events() {

    while (true) {
        auto [event, err] = events.read();
        if (err) { break; }

        // If the event has a sender, deliver it directly to it
        // This is used for view subtree updating

        if (event->get_receiver()) {
            event->get_receiver()->on_event(event);
        }

        else if (event->get_sender()) {
            switch(event->get_type()) {
                case Event::DELETE_LATER:
                    delete event->get_sender();
                    break;
                default:
                    event->get_sender()->on_event(event);
            }
        }

        // Certain events have no sender but go directly to the
        // "focused" widget
        else if(event->get_type() == Event::KEYBOARD_EVENT &&
                Widget::get_focused_widget()) {
            Widget::get_focused_widget()->on_event(event);
        }
        else if ( event->get_type() == Event::MOUSE_EVENT) {
            MouseEvent* me = static_cast<MouseEvent*>(event);
            Widget::set_mouse_position(me->get_mouse_pos());
            if (Widget* wid = Widget::get_widget_under_mouse(); wid) {
                wid->on_event(event);
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
