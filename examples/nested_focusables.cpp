#include "notcute/widget.hpp"
#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/focus_stack.hpp>

using namespace notcute;

void style_widget_focused(Widget* wid) {
    if (FrameWidget* frame = dynamic_cast<FrameWidget*>(wid); frame) {
        frame->set_frame_fg(notcute::DARK_YELLOW);
        frame->set_frame_title_fg(notcute::BLUE);
    }
}

void style_widget_notfocused(Widget* wid) {
    if (FrameWidget* frame = dynamic_cast<FrameWidget*>(wid); frame) {
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
    }

    // One way to capture focus changes is via the on_event override
    bool on_event(Event* e) override {
        switch(e->get_type()) {
            case Event::FOCUS_IN:
                style_widget_focused(this);
                break;
            case Event::FOCUS_OUT:
                style_widget_notfocused(this);
                break;
            default:
                break;
        }
        return FrameWidget::on_event(e);
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch(e->get_key()) {
            case NCKEY_ESC:
                // this function will traverse up the view tree until
                // we find a parent that has a focus stack, in this case,
                // OuterWidget
                if (FocusStack* fs = get_next_parent_focus_stack(); fs) {
                    fs->pop_focus();
                }
                return true;
        }

        return FrameWidget::on_keyboard_event(e);
    }
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

        // Push this to the focus stack
        use_focus_stack();
    }

    // Another way to capture focus events, FOCUS_IN and FOCUS_OUT
    // each have their own dedicated callback
    bool on_focus_in_event(FocusEvent* e) override {
        style_widget_focused(this);
        return true;
    }
    bool on_focus_out_event(FocusEvent* e) override {
        style_widget_notfocused(this);
        return true;
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch(e->get_key()) {
            case NCKEY_ENTER:
                get_focus_stack()->push(ui.inner);
                return true;
        }

        return FrameWidget::on_keyboard_event(e);
    }
private:
    struct ui{
        InnerWidget* inner;
    }ui;
};

int main() {
    MainWindow mw;
    mw.get_layout()->add_widget(new OuterWidget(&mw));
    mw.show();
}
