#pragma once

#include <queue>
#include <functional>
#include <cstdint>
#include <ncpp/NotCurses.hh>
#include "util.hpp"
#include "rect.hpp"

namespace notcute {


class Widget;

class Event {
public:
    enum EventType {
        DRAW,
        RESIZE,
        KEYBOARD_EVENT,
        MOUSE_EVENT,

        USER_EVENT,
    };

    Event(Widget* sender, EventType t)
        : sender(sender),
        type(t) {}
    virtual ~Event() = default;

    EventType get_type() const { return type; }
    Widget* get_sender() { return sender; }

private:
    Widget* sender;
    EventType type;
};

class DrawEvent : public Event {
public:
    DrawEvent(Widget* w)
        :Event(w, EventType::DRAW)
    {
    }
};

class KeyboardEvent : public Event {
public:
    KeyboardEvent(Widget* w, const ncinput& ni)
        :Event(w, EventType::KEYBOARD_EVENT)
        ,ni(ni)
    {
    }

    uint32_t get_key() { return ni.id; }
    const ncinput& get_ncinput() { return ni; }

private:
    ncinput ni = {};
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
