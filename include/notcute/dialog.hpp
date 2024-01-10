#pragma once

#include "main_window.hpp"
#include "frame_widget.hpp"
#include "edittext.hpp"
#include "text_widget.hpp"
#include "widget.hpp"

namespace notcute {

class FullScreenDialog : public MainWindow {
public:
    FullScreenDialog(const std::string& prompt)
        : prompt_text(prompt)
    {
    }

    ~FullScreenDialog() {
    }

    void exec() {
        set_name("window");
        notcute::FrameWidget* frame = new notcute::FrameWidget(this);
        frame->set_name("frame");
        frame->set_layout(new notcute::VBoxLayout(3,25));
        frame->get_layout()->set_behave(LAY_CENTER);

        edittext = new notcute::EditText(frame);
        edittext->set_name("edittext");
        edittext->get_layout()->set_behave(LAY_FILL);
        edittext->get_layout()->set_margins_ltrb(1,1,1,1);
        std::string submition;
        edittext->entry_submitted.connect([&](const std::string& entry){
                notcute::log_debug(std::string(entry));
                submition = entry;
                accepted(entry);
                done_showing();
            });

        notcute::TextWidget* prompt = new notcute::TextWidget(prompt_text, frame);
        prompt->get_layout()->set_behave(LAY_CENTER);
        prompt->set_name("prompt");

        get_layout()->add_widget(prompt);
        get_layout()->add_widget(frame);
        frame->get_layout()->add_widget(edittext);
        frame->get_layout()->set_behave(LAY_CENTER);

        set_focus_policy(FocusPolicy::FOCUS);
        set_focus();

        show();
    }

    bool on_keyboard_event(KeyboardEvent* e) override {

        if (edittext->on_keyboard_event(e)) {
            return true;
        }

        switch (e->get_key()) {
            case NCKEY_ESC:
                was_canceled = true;
                canceled();
                done_showing();
                return true;
        }

        return false;
    }

    const std::string& get_entry() const { return edittext->get_contents(); }

    bool get_was_canceled() const { return was_canceled; }

    signal<void(const std::string&)> accepted;
    signal<void()> canceled;

private:
    EditText* edittext = nullptr;
    std::string prompt_text;
    bool was_canceled = false;
};


}
