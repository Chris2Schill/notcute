// #pragma once
//
// #include <functional>
// #include <vector>
// #include <typeindex>
// #include <typeinfo>
// #include <unordered_map>
// #include <memory>
// #include <algorithm>
//
// #if __GNUC__ > 6
//     #include <any>
//     using stdany = std::any;
//     #define ANY_CAST std::any_cast
// #else
//     #include <experimental/any>
//     using stdany = std::experimental::any;
//     #define ANY_CAST std::experimental::any_cast
// #endif
//
// /* cschilling@avtsim.com
//  *
//  * To subscribe to an event:
//  *
//  *     // Given..
//  *     struct MyCustomEvent{ int value;};
//  *     void SubscriberClass::handle(const MyCustomEvent& event) {}
//  *     void freeFunction() {}
//  *
//  *     // Usage
//  *     event_bus::Dispatcher bus;
//  *
//  *     bus.subscribe<MyCustomEvent>(this, &SubscriberClass::handle);            // member function
//  *     bus.subscribe<MyCustomEvent>(this, [this](const MyCustomEvent& e){} );   // lambda with explicit receiver
//  *     bus.subscribe<MyCustomEvent>([this](const MyCustomEvent& e){} );         // lambda with no explicit receiver
//  *     bus.subscribe<MyCustomEvent>(freeFunction);                              // free function
//  *
//  *     bus.unsubscribe<MyCustomEvent>(this);                                    // member function/lambda with explicit receiver
//  *     bus.unsubscribe<MyCustomEvent>([this](const MyCustomEvent& e){} );       // lambdas with no explicit receiver
//  *     bus.unsubscribe<MyCustomEvent>(this, [this](const MyCustomEvent& e){} ); // lambdas with explicit receiver
//  *     bus.unsubscribe<MyCustomEvent>(freeFunction);                            // free function
//  *
//  *
//  * To Post an event:
//  *
//  *     MyCustomEvent e;
//  *     e.value = 10;
//  *     bus.post(e);
//  *
//  * Notes:
//  *     Wrapping type-erased std::function in type-erased std::any might
//  *     not be the most efficient approach. Eliminating one
//  *     type-erased indirection might be an aveneue for further optimization.
//  *     That said, this is a minimal implementation and fast enough for most
//  *     use cases.
//  */
//
// #define EVENT_SUBSCRIBE event_bus::Dispatcher::getInstance().subscribe
// #define EVENT_POST event_bus::Dispatcher::getInstance().post
//
// namespace event_bus
// {
//
// template<typename T, typename... U>
// auto getAddress(std::function<T(U...)> f)
// {
//     typedef T(fnType)(U...);
//     fnType **funcPtr = f.template target<fnType*>();
//     return funcPtr;
// }
//
//
// template<typename Event>
// struct Subscriber
// {
//     using method_hash = std::pair<intptr_t, intptr_t>;
//
//     template<typename Receiver>
//     Subscriber(Receiver* obj, void(Receiver::*method)(const Event&))
//         : callback(std::bind(method, obj, std::placeholders::_1))
//         ,id(reinterpret_cast<intptr_t>(obj),
//             reinterpret_cast<intptr_t>(getAddress(callback)))
//     {}
//
//     template<typename Receiver, typename Callable>
//     Subscriber(Receiver* obj, Callable&& callable)
//         : callback(callable)
//         , id(reinterpret_cast<intptr_t>(obj),
//              reinterpret_cast<intptr_t>(getAddress(callback)))
//     {}
//
//     template<typename Callable>
//     Subscriber(Callable&& callable)
//         : callback(std::function<void(const Event&)>(callable))
//     {
//         id = method_hash(intptr_t(0),
//                          reinterpret_cast<intptr_t>(getAddress(callback)));
//     }
//
//     std::function<void(const Event&)> callback;
//     method_hash id = {0,0};
// };
//
// class Dispatcher
// {
// protected:
//
//     std::unordered_map<std::type_index, std::vector<stdany>> observers;
//
// public:
//
//     // Overload for free-functions, static methods, and lambdas (no explicit receiver)
//     template<class Event, typename Callable>
//     void subscribe(Callable&& slot)
//     {
//         observers[typeid(Event)].emplace_back(Subscriber<Event>((void*)nullptr, slot)); 
//     }
//
//     // Overload for member functions, and lambdas with an explicit receiver
//     template<class Event, class Receiver, class Callable>
//     void subscribe(Receiver* obj, Callable&& callable)
//     {
//         observers[typeid(Event)].emplace_back(Subscriber<Event>(obj,callable));
//     }
//
//     template<typename T>
//     void post(const T& event) const
//     {
//         auto obs = observers.find(typeid(T));
//
//         if (obs != observers.end())
//         {
//             for (stdany observer : obs->second)
//             {
//                 std::invoke(ANY_CAST<Subscriber<T>&>(observer).callback,
//                             std::forward<const T&>(event));
//             }
//         }
//     }
//
//     // Unsubscribe a freefunction/static receiver
//     template<class Event>
//     void unsubscribe(void (slot)(const Event&))
//     {
//         auto obs = observers.find(typeid(Event));
//
//         if (obs != observers.end())
//         {
//             auto& vec = obs->second;
//             auto iter = std::remove_if(vec.begin(), vec.end(),
//                 [&slot](stdany f)
//                 {
//                     return slot == *getAddress(ANY_CAST<Subscriber<Event>>(&f)->callback);
//                 });
//             vec.erase(iter, vec.end());
//         }
//     }
//
//     // Unsubscribe a class instance of Receiver from event Event
//     // (including lambdas with explicit receivers)
//     template<class Event, class Receiver>
//     void unsubscribe(Receiver* obj)
//     {
//         auto obs = observers.find(typeid(Event));
//
//         if (obs != observers.end())
//         {
//             auto& vec = obs->second;
//             auto iter = std::remove_if(vec.begin(), vec.end(),
//                 [&obj](stdany f)
//                 {
//                     return reinterpret_cast<intptr_t>(obj) == ANY_CAST<Subscriber<Event>>(&f)->id.first;
//                 });
//             vec.erase(iter, vec.end());
//         }
//     }
//
//     // This overload will remove ALL subscribers of a particular
//     // event that did NOT specify an explicit reciever. (i.e using single parameter callable overload).
//     template<class Event, typename Callable>
//     void unsubscribe(Callable&& slot)
//     {
//         auto obs = observers.find(typeid(Event));
//
//         if (obs != observers.end())
//         {
//             auto& vec = obs->second;
//             auto iter = std::remove_if(vec.begin(), vec.end(),
//                 [slot](stdany f)
//                 {
//                     Subscriber<Event>& sub = ANY_CAST<Subscriber<Event>&>(f);
//                     return sub.id.first == reinterpret_cast<intptr_t>(nullptr) &&
//                             reinterpret_cast<intptr_t>(getAddress(std::function<void(const Event&)>(slot))) ==
//                             sub.id.second;
//                 });
//             vec.erase(iter, vec.end());
//         }
//     }
//
//
//     int getObserverCount() const
//     {
//         int count = 0;
//         for (auto iter : observers)
//         {
//             for (auto obs : iter.second)
//             {
//                 count++;
//             }
//         }
//         return count;
//     }
//     // NOTE: this class is not a singleton, it may be created and passed around
//     // to utilize separate event buses. However this is a convenience method for
//     // simple projects that need only a single bus.
//     static Dispatcher& getInstance()
//     {
//         static Dispatcher dispatcher;
//         return dispatcher;
//     }
//
//
// };
//
// using DispatcherPtr = std::shared_ptr<event_bus::Dispatcher>;
//
//
// } // end namespace event_bus
