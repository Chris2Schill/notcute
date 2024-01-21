#pragma once


namespace notcute {

class Widget;
class BoxLayout;
using Layout = BoxLayout;

class LayoutItem {
public:
    virtual ~LayoutItem() = default;

    virtual Widget* get_widget() {
        return nullptr;
    }

    // Used to check if a LayoutItem is in fact
    // a Layout. 
    virtual Layout* get_layout() { return nullptr; }

    // If this is called then, deletion
    // of the layout item should not cause
    // a deletion of any owned objects.
    virtual void release_ownership() {}
};


class WidgetItem : public LayoutItem {
public:
    WidgetItem(Widget* w) : wid(w) {}
    ~WidgetItem();

    virtual Widget* get_widget() override {
        return wid;
    }

    void release_ownership() override;

    Layout* get_layout() override;

private:
    Widget* wid;
};

}
