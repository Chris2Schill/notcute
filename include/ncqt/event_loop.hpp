#pragma once

#include <queue>
#include <functional>
#include <any>
#include <cstdint>

namespace ncqt {


class Widget;

class Event {
public:
    enum EventType {
        DRAW,
        LAYOUT_REQUEST,
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

    void process_events() {
        while (!events.empty()) {
            Event* e = events.front();
            events.pop();
            for (auto& sub : subscribers) {
                sub.callback(e);
            }
            delete e;
        }
    }

    void post(Event* e) {
        events.push(e);
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

    std::vector<Subscriber<Event>> subscribers;

    std::queue<Event*> events;
};

}
