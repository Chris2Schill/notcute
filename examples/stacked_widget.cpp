#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/stacked_widget.hpp>
#include <notcute/text_widget.hpp>

using namespace notcute;

class Controller : public MainWindow {
public:
    Controller()
    {
        ui.sw = new StackedWidget(this);
        for (int i = 0; i < 3; ++i) {
            int r = (rand() % 256);
            int g = (rand() % 256);
            int b = (rand() % 256);
            FrameWidget* frame = new FrameWidget;
            frame->set_title("Press Enter to go to the next child");
            frame->set_layout(new VBoxLayout);
            frame->debug_set_plane_color(r,g,b);


            ui.sw->add_widget(frame);

        }
        get_layout()->add_widget(ui.sw);
    }

    bool on_keyboard_event(KeyboardEvent* e) override {
        if (e->get_key() == NCKEY_ENTER) {
            ui.sw->next_child();
        }
        return MainWindow::on_keyboard_event(e);
    }

private:
    struct ui{
        StackedWidget* sw;
    }ui;

};

int main() {
    Controller mw;
    mw.show();
}
