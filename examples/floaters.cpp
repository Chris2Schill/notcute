#include "notcute/notcute.hpp"
#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/tab_widget.hpp>
#include <notcute/text_widget.hpp>
#include <notcute/float_widget.hpp>
#include <openssl/rand.h>

using namespace notcute;


void run() {
    srand(time(NULL));

    MainWindow mw;

    for (int i = 0; i < 3; ++i) {
        auto frame = new FrameWidget;
        frame->set_layout(new VBoxLayout);
        mw.get_layout()->add_widget(frame);
    }

    Rect rect = mw.get_geometry();

    // Floaters will float above its parent. They will also 
    // default to the center of terminal window.
    // Floaters do not get added to a layout.
    std::vector<Widget*> floaters(100, nullptr);
    for (int i = 0; i < floaters.size(); ++i) {
        FloatWidget* floater = new FloatWidget(&mw);
        floater->set_layout(new VBoxLayout(7,11));

        int r = (rand() % 256);
        int g = (rand() % 256);
        int b = (rand() % 256);
        floater->debug_set_plane_color(r,g,b);

        floater->get_layout()->add_widget(
                new Label("FLOATER", floater));

        Point pos = {
            .x = static_cast<int>(rand() % rect.width()),
            .y = static_cast<int>(rand() % rect.height()),
        };
        floater->move(pos); // Coords are relative to parent
        floaters[i] = floater;
    }

    mw.show();

    // Because floaters are not part of the layout heirarchy and thus
    // are not child nodes in the view tree, we must take care of 
    // freeing their memory
    for (Widget* wid : floaters) {
        delete wid;
    }
}

int main() {
    run();
    report_closing_diagnostic();
}
