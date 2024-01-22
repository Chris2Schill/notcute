#include "notcute/widget.hpp"
#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/focus_stack.hpp>

using namespace notcute;

void on_focus_node_focused(FocusNode* node) {
    if (FrameWidget* frame = dynamic_cast<FrameWidget*>(node->widget); frame) {
        frame->set_frame_fg(notcute::DARK_YELLOW);
        frame->set_frame_title_fg(notcute::BLUE);
    }
}

void on_focus_node_notfocused(FocusNode* node) {
    if (FrameWidget* frame = dynamic_cast<FrameWidget*>(node->widget); frame) {
        frame->set_frame_fg(notcute::WHITE);
        frame->set_frame_title_fg(notcute::WHITE);
    }
}

class InnerWidget : public FrameWidget {
public:
    InnerWidget(Widget* parent = nullptr)
        : FrameWidget(parent)
    {
        set_layout(new VBoxLayout(20,40));
        set_title("Inner -> Esc to pop focus");
        set_focus_policy(FocusPolicy::FOCUS);

        // So that it lays in the center of its parent.
        // This effectively clears the default LAY_FILL
        // flag that VBoxLayout comes with
        get_layout()->set_behave(LAY_CENTER);

        focused_node = setup_focus_graph(
                { {this}, },
                &on_focus_node_focused,
                &on_focus_node_notfocused
            ).front();
    }

    ~InnerWidget() {
        delete_focus_graph(focused_node);
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch(e->get_key()) {
            case NCKEY_ESC:
                // Pop the focus stack to return to whatever
                // had focus last
                focus_stack_pop();
                break;
        }

        return FrameWidget::on_keyboard_event(e);
    }

    void take_focus() {
        focus_stack_push({this, &focused_node});
    }
private:
    FocusNode* focused_node;
};

class OuterWidget : public FrameWidget {
public:
    OuterWidget(Widget* parent = nullptr)
        : FrameWidget(parent)
    {
        set_layout(new VBoxLayout);
        set_title("Outer -> Enter to focus Inner");
        set_focus_policy(FocusPolicy::FOCUS);

        ui.inner = new InnerWidget(this);
        get_layout()->add_widget(ui.inner);

        // Here we are setting up a focus graph with only one node, for use
        // with the focus stack
        focused_node = setup_focus_graph(
                { {this}, },
                &on_focus_node_focused,
                &on_focus_node_notfocused
            ).front();

        // Push the focus graph to the focus stack
        focus_stack_push({this, &focused_node});
    }

    ~OuterWidget() {
        delete_focus_graph(focused_node);
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch(e->get_key()) {
            case NCKEY_ENTER:
                ui.inner->take_focus();
                break;
        }

        return FrameWidget::on_keyboard_event(e);
    }
private:
    struct ui{
        InnerWidget* inner;
    }ui;
    FocusNode* focused_node;
};

int main() {
    MainWindow mw;
    mw.get_layout()->add_widget(new OuterWidget(&mw));
    mw.show();
}
