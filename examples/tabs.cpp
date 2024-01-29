#include <notcute/main_window.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/tab_widget.hpp>
#include <notcute/text_widget.hpp>
#include <openssl/rand.h>

using namespace notcute;

int main() {
    srand(time(NULL));

    MainWindow mw;

    TabWidget* tw = new TabWidget;
    for (int i = 0; i < 3; ++i) {
        int r = (rand() % 256);
        int g = (rand() % 256);
        int b = (rand() % 256);
        Widget* wid = new Widget;
        wid->set_layout(new VBoxLayout);
        wid->debug_set_plane_color(r,g,b);

        tw->add_tab("Tab "+std::to_string(i),wid);
    }

    mw.get_layout()->add_widget(tw);

    tw->set_focus();

    mw.show();
}
