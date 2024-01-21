#pragma once

#include <string>
#include <notcute/widget.hpp>

namespace notcute {

class Widget;

class ListItem {
public:
    virtual ~ListItem() = default;

    virtual Widget* get_widget() { return nullptr; }

    virtual std::string to_string() const = 0;
};


template<typename T>
std::string to_string(const T&);

// A generic ListItem that can carry any T.
// T must have template specialization
//
//     template<>
//     std::string notcute::to_string<T>(const T&) { return "whatever"; }
//
// defined that specifies what text will be shwon for that list item
// or the compiler will complain
template<typename T>
class ListItem_t : public ListItem {
public:
    ListItem_t(T t) : obj(t) {}

    virtual std::string to_string() const override {
        return notcute::to_string<T>(obj);
    }

    T& get_item() {
        return obj;
    }

private:
    T obj;
};

// Helper function to extract item T from a ListItem_t.
// Will throw an exception if the dynamic cast fails
template<typename T>
T list_widget_item_t_get_item(notcute::ListItem* item) {
    ListItem_t<T>& obj = dynamic_cast<ListItem_t<T>&>(*item);
    return obj.get_item();
}

// For use when you just want a simple list of strings. No
// extra widgets required
class ListTextItem : public ListItem {
public:
    ListTextItem(const std::string& text)  
        : text(text)
    {
    }

    virtual std::string to_string() const override { return text; }

private:
    std::string text;
};

// To store widgets in list items
class ListWidgetItem : public ListItem {
public:
    ListWidgetItem(Widget* w) : wid(w) {}
    ~ListWidgetItem() { delete wid; }


    virtual Widget* get_widget() override { return wid; }

    std::string to_string() const override {
        return wid->to_string();
    }

private:
    Widget* wid = nullptr;
};

}

