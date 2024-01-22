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

        ui.list = new ListWidget(this);
        ui.list->set_title("Up/Down or j/k to scroll. Enter to select an item. Esc to change focus");

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

    ListWidget* get_list() { return ui.list; }

private:

    struct ui{
        ListWidget* list;
        Widget*     selected_items;
    }ui;
};

class Orchestrator: public FrameWidget {
public:
    Orchestrator(Widget* parent = nullptr)
        : FrameWidget(parent)
    {
        set_layout(new VBoxLayout);
        set_title("Orchestrator");

        // We need to make sure this widget can receive focus
        set_focus_policy(FocusPolicy::FOCUS);

        ui.container = new Widget(this);
        ui.container->set_layout(new VBoxLayout);
        ui.container->get_layout()->set_margins_ltrb(1,1,1,1);

        ui.wid1 = new CustomWidget(ui.container);
        ui.wid2 = new CustomWidget(ui.container);

        ui.container->get_layout()->add_widget(ui.wid1);
        ui.container->get_layout()->add_widget(ui.wid2);

        get_layout()->add_widget(ui.container);

        std::vector<FocusNode*> nodes = setup_focus_graph(
            {
                {ui.wid1->get_list()},
                {ui.wid2->get_list()},
            },
            [this](FocusNode* n){ on_focus_node_focused(n); },  // called for a node when it gains focus
            [this](FocusNode* n){ on_focus_node_notfocused(n); }// called for a node when it loses focus
        ); 

        // Start with wid1 as the active focused node
        focused_node = nodes.front();
        focused_node->on_focus();

        // The orchestrator will start with focus and will delegate input events
        // to the current focused node
        set_focus();
    }

    ~Orchestrator() {
        delete_focus_graph(focused_node);
    }

    // Change some colors to indicate focus
    void on_focus_node_focused(FocusNode* node) {
        if (ListWidget* list = dynamic_cast<ListWidget*>(node->widget); list) {
            list->set_frame_fg(notcute::DARK_YELLOW);
            list->set_frame_title_fg(notcute::BLUE);
        }
    }

    // Change some colors to indicate focus
    void on_focus_node_notfocused(FocusNode* node) {
        if (ListWidget* list = dynamic_cast<ListWidget*>(node->widget); list) {
            list->set_frame_fg(notcute::WHITE);
            list->set_frame_title_fg(notcute::WHITE);
        }
    }

    bool on_keyboard_event(KeyboardEvent *e) override {

        // You can do this however you like, in this example we
        // will delegate input events to the focus graph before
        // the orchestrator attempts to handle it.
        if (focused_node->widget->on_keyboard_event(e)) {
            return true;
        }

        switch(e->get_key()) {
            case NCKEY_ESC:
                focused_node = focused_node->focus_next();
                return true;
        }

        return FrameWidget::on_keyboard_event(e);
    }

private:
    struct ui{
        Widget*       container;
        CustomWidget* wid1;
        CustomWidget* wid2;
    }ui;

    FocusNode* focused_node;
};

int main() {

    MainWindow mw;
    mw.get_layout()->add_widget(new Orchestrator(&mw));
    mw.show();
}
