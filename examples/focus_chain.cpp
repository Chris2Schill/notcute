#include "notcute/widget.hpp"
#include <notcute/main_window.hpp>
#include <notcute/layout.hpp>
#include <notcute/list_widget.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/text_widget.hpp>
#include <notcute/focus_stack.hpp>

using namespace notcute;

// This example will build off the the custom_widgets example to show
// how you can transfer focus between widgets using the focus stack

class CustomWidget : public FrameWidget {
public:
    CustomWidget(Widget* parent = nullptr)
        :FrameWidget(parent)
    {
        set_layout(new HBoxLayout);
        set_focus_policy(FocusPolicy::FOCUS);

        ui.list = new ListWidget(this);
        ui.list->set_title("Up/Down or j/k to scroll. Enter to select an item. Tab to change focus");

        for (int i = 0; i < 10; ++i) {
            ui.list->add_text("LIST ITEM: " + std::to_string(i));
        }

        ui.selected_items = new FrameWidget(this);
        ui.selected_items->set_layout(new VBoxLayout);

        ui.list->item_selected.connect([this](ListItem* item){
                std::string str = item->to_string();
                Label* label = new Label(str, ui.selected_items);
                ui.selected_items->get_layout()->add_widget(label);
                label->get_layout()->set_margins_ltrb(1,0,1,0);
            });

        get_layout()->add_widget(ui.list);
        get_layout()->add_widget(ui.selected_items);
    }

    // Change some colors to indicate focus
    bool on_focus_in_event(FocusEvent* e) override {
        ui.list->set_frame_fg(notcute::DARK_YELLOW);
        ui.list->set_frame_title_fg(notcute::BLUE);
        return true;
    }


    // Change some colors to indicate focus
    bool on_focus_out_event(FocusEvent* e) override {
        ui.list->set_frame_fg(notcute::WHITE);
        ui.list->set_frame_title_fg(notcute::WHITE);
        return true;
    }

    // When we press tab, the list does not handle that key.
    // For tab specifically, it attempts to tell the parent
    // to focus the next node in its focus chain.
    bool on_keyboard_event(KeyboardEvent* e) override {
        if (ui.list->on_keyboard_event(e)) {
            return true;
        }
        return FrameWidget::on_keyboard_event(e);
    }

    ListWidget* get_list() { return ui.list; }

private:

    struct ui{
        ListWidget* list;
        Widget*     selected_items;
    }ui;
};

class Container: public FrameWidget {
public:
    Container(Widget* parent = nullptr)
        : FrameWidget(parent)
    {
        set_layout(new VBoxLayout);
        set_title("Orchestrator");

        ui.wid1 = new CustomWidget(this);
        ui.wid2 = new CustomWidget(this);

        get_layout()->add_widget(ui.wid1);
        get_layout()->add_widget(ui.wid2);

        // Setup the focus chain. You can call this multiple times to add more
        // connections. Here we are saying wid2 comes after wid1. The focus chain
        // will maintain a circular invariant for you

        set_tab_order(ui.wid1, ui.wid2);

        // start with wid1 focused
        ui.wid1->set_focus();
    }

private:
    struct ui{
        CustomWidget* wid1;
        CustomWidget* wid2;
    }ui;
};

int main() {

    MainWindow mw;
    mw.get_layout()->add_widget(new Container(&mw));
    mw.show();
}
