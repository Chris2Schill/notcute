#pragma once

#include "frame_widget.hpp"
#include "notcute/widget.hpp"
#include "stacked_widget.hpp"
#include "colors.hpp"

namespace notcute {

class TabWidget : public FrameWidget {
public:
    TabWidget(Widget* parent = nullptr)
        :FrameWidget(parent)
    {
        set_layout(new VBoxLayout);
        set_focus_policy(FocusPolicy::FOCUS);

        ui.container = new Widget(this);
        ui.container->set_layout(new VBoxLayout);
        ui.container->get_layout()->set_margins_ltrb(1,1,1,1);

        ui.sw = new StackedWidget(ui.container);
        ui.container->get_layout()->add_widget(ui.sw);
        get_layout()->add_widget(ui.container);
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        switch(e->get_key()) {
            case '[':
                ui.sw->prev_child();
                return true;
            case ']':
                ui.sw->next_child();
                return true;
            default:
                return FrameWidget::on_keyboard_event(e);
        }
    }

    void draw_title(ncpp::Plane* p) override {
        if (tabs.empty()) { return; }

        int x = 2;

        for (int i = 0; i < tabs.size(); ++i) {

            auto chans = channels_from_fgbg(WHITE, {});
            if (i == ui.sw->get_index()) {
                chans = channels_from_fgbg(GREEN, {});
            }

            p->set_channels(chans);
            p->putstr(0,x,tabs[i].c_str());
            x += tabs[i].size();


            if (i < tabs.size()-1) {
                p->set_channels(0);
                p->putstr(0,x," - ");
                x += 3;
            }
        }
    }

    void add_tab(const std::string& tab_text, Widget* wid) {
        tabs.push_back(tab_text);
        ui.sw->add_widget(wid);
        redraw();
    }

    void next_tab() {
        ui.sw->next_child();
    }

private:
    std::vector<std::string> tabs;

    struct ui{
        Widget*        container;
        StackedWidget* sw;
    }ui;

};

}
