#pragma once

#include <functional>
#include <cstdint>
#include <ncpp/NotCurses.hh>
#include "rect.hpp"
#include "util.hpp"

namespace notcute {


class Widget;

class Event {
public:
    enum EventType {
        DRAW,
        RESIZE,
        KEYBOARD_EVENT,
        MOUSE_EVENT,
        DELETE_LATER,
        FOCUS_IN,
        FOCUS_OUT,

        USER_EVENT,
    };

    Event(EventType t, Widget* sender = nullptr, Widget* receiver = nullptr)
        : sender(sender),
        receiver(receiver),
        type(t) {}
    virtual ~Event() = default;

    EventType get_type() const { return type; }
    Widget* get_sender() { return sender; }
    Widget* get_receiver() { return receiver; }

private:
    Widget* sender = nullptr;
    Widget* receiver = nullptr;
    EventType type;
};

class DrawEvent : public Event {
public:
    DrawEvent(Widget* w)
        :Event(EventType::DRAW, w, w)
    {
    }
};

class KeyboardEvent : public Event {
public:
    KeyboardEvent(const ncinput& ni)
        :Event(EventType::KEYBOARD_EVENT)
        ,ni(ni)
    {
    }

    uint32_t get_key() { return ni.id; }
    ncintype_e get_evtype() { return ni.evtype; }
    const ncinput& get_ncinput() { return ni; }

private:
    ncinput ni = {};
};

class MouseEvent : public Event {
public:
    MouseEvent(const ncinput& ni)
        :Event(EventType::MOUSE_EVENT)
        ,ni(ni)
    {
    }

    // evtype can by one of
    // NCTYPE_UNKNOWN,
    // NCTYPE_PRESS,
    // NCTYPE_REPEAT,
    // NCTYPE_RELEASE,

    uint32_t get_button() { return ni.id; }
    ncintype_e get_evtype() { return ni.evtype; }
    Point    get_mouse_pos() { return {.x = ni.x, .y = ni.y};}
    const ncinput& get_ncinput() { return ni; }

private:
    ncinput ni = {};
};

class FocusEvent : public Event {
public:
    FocusEvent(Event::EventType e, Widget* sender, Widget* receiver)
        :Event(e, sender, receiver)
    {
        assert(e == FOCUS_IN || e == FOCUS_OUT);
    }

    bool lost_focus() { return get_type() == FOCUS_OUT; }
    bool has_focus() { return get_type() == FOCUS_IN; }
};

// class ResizeEvent : public Event {
// public:
//     ResizeEvent(Widget* w)
//         :Event(w, EventType::RESIZE)
//     {
//     }
// };

template<typename T, typename... U>
auto getAddress(std::function<T(U...)> f)
{
    typedef T(fnType)(U...);
    fnType **funcPtr = f.template target<fnType*>();
    return funcPtr;
}


template<typename E>
struct Subscriber
{
    using method_hash = std::pair<intptr_t, intptr_t>;

    template<typename Receiver>
    Subscriber(Receiver* obj, void(Receiver::*method)(E*))
        : callback(std::bind(method, obj, std::placeholders::_1))
        ,id(reinterpret_cast<intptr_t>(obj),
            reinterpret_cast<intptr_t>(getAddress(callback)))
    {}

    template<typename Receiver, typename Callable>
    Subscriber(Receiver* obj, Callable&& callable)
        : callback(callable)
        , id(reinterpret_cast<intptr_t>(obj),
             reinterpret_cast<intptr_t>(getAddress(callback)))
    {}

    template<typename Callable>
    Subscriber(Callable&& callable)
        : callback(std::function<void(E*)>(callable))
    {
        id = method_hash(intptr_t(0),
                         reinterpret_cast<intptr_t>(getAddress(callback)));
    }

    std::function<void(E*)> callback;
    method_hash id = {0,0};
};

class EventLoop {
public:

    void process_events();

    void post(Event* e) {
        events.write(e);
    }

    // template<class E, typename Callable>
    // void subscribe(Callable&& slot)
    // {
    //     // subscribers.emplace_back(Subscriber<E>((void*)nullptr, slot)); 
    // }

    template<class E, class Receiver, class Callable>
    void subscribe(Receiver* obj, Callable&& callable)
    {
        subscribers.emplace_back(Subscriber<E>(obj,callable));
    }

    static EventLoop* get_instance() {
        static EventLoop* el = new EventLoop;
        return el;
    }

    void set_showing_widget(Widget* w) { showing_widget = w; }
    Widget* get_showing_widget() const { return showing_widget; }

private:

    std::vector<Subscriber<Event>> subscribers;

    util::NonBlockingChannel<Event*> events;
    notcute::Widget* showing_widget = nullptr;
};

}
