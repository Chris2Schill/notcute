#pragma once

#include "frame_widget.hpp"
#include "text_widget.hpp"
#include "colors.hpp"

namespace notcute {

class Button : public FrameWidget{
public:

    Button(Widget* parent = nullptr)
        :FrameWidget(parent)
    {
        set_layout(new VBoxLayout);
        get_layout()->set_behave(LAY_CENTER);
        set_focus_policy(FocusPolicy::FOCUS);
        set_mouse_events_enabled(true);

        ui.label = new Label(this);
        ui.label->get_layout()->set_margins_ltrb(1,1,1,1);
        get_layout()->add_widget(ui.label);

        set_colors(fg_nothovered, bg_nothovered);
    }

    Button(const std::string& label, Widget* parent = nullptr)
        : Button(parent)
    {
        set_label(label);
    }

    void set_label(const std::string& text) {
        ui.label->set_text(text);
        Rect r = get_geometry();
        r.set_height(3);
        r.set_width(ui.label->get_text().size()+2);
        set_geometry(r);
        redraw();
    }

    void on_hover_enter() override {
        FrameWidget::on_hover_enter();
        set_colors(fg_hovered, bg_hovered);
    }

    void on_hover_leave() override {
        FrameWidget::on_hover_leave();
        set_colors(fg_nothovered, bg_nothovered);
    }

    bool on_mouse_event(MouseEvent* e) override {
        switch(e->get_button()) {
            case NCKEY_BUTTON1: {
                switch(e->get_evtype()) {
                    case NCTYPE_PRESS: {
                        // Is this check needed? do more testing.
                        // Maybe the backend input system needs properly
                        // only send on press per actual press
                        // if (!is_pressed) {
                            set_colors(fg_pressed, bg_pressed);
                            is_pressed = true;
                            clicked();
                            pressed();
                            redraw();
                        // }
                        return true;
                    }
                    case NCTYPE_RELEASE: {
                        // if (is_pressed) {
                            log_debug("RELEASED");
                            Color fg = is_under_mouse()
                                        ? fg_hovered
                                        : fg_nothovered;
                            Color bg = is_under_mouse()
                                        ? bg_hovered
                                        : bg_nothovered;
                            set_colors(fg, bg);
                            is_pressed = false;
                            released();
                        // }
                        return true;
                    }
                    default:
                        break;
                }
            }
        }

        return FrameWidget::on_mouse_event(e);
    }


    // signals
    signal<void()> clicked;
    signal<void()> pressed;
    signal<void()> released;

    void set_fgbg_hovered(Color fg, Color bg) {
        fg_hovered = fg;
        bg_hovered = bg;
    }

    void set_fgbg_nothovered(Color fg, Color bg) {
        fg_nothovered = fg;
        bg_nothovered = bg;
    }

    void set_fgbg_pressed(Color fg, Color bg) {
        fg_pressed = fg;
        bg_pressed = bg;
    }

    bool get_pressed() const { return is_pressed; }

private:
    void set_colors(Color fg, Color bg) {
        set_frame_fg(fg);
        set_frame_bg(bg);
        ui.label->set_fg(fg);
        ui.label->set_bg(bg);
    }

    Color fg_nothovered = {255,255,255,NCALPHA_OPAQUE};
    Color bg_nothovered = {000,000,000,NCALPHA_TRANSPARENT};
    Color fg_hovered    = {255,000,000,NCALPHA_OPAQUE};
    Color bg_hovered    = {255,255,255,NCALPHA_TRANSPARENT};
    Color fg_pressed    = {180,000,000,NCALPHA_OPAQUE};
    Color bg_pressed    = {255,000,000,NCALPHA_TRANSPARENT};

    struct ui{
        Label* label = nullptr;
    }ui;

    bool is_pressed = false;
};

}
