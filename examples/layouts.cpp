#include <notcute/main_window.hpp>
#include <notcute/layout.hpp>
#include <notcute/frame_widget.hpp>

using namespace notcute;

int main() {

    // MainWindow will automatically fill the terminal screen
    MainWindow mw;

    // Main window starts with a VBoxLayout.
    // Lets change it to be horizontal
    mw.get_layout()->set_contain(LAY_ROW);

    // Lets add two FrameWidgets to the layout to do split screen
    for (int i = 0; i < 2; ++i) {
        FrameWidget* frame = new FrameWidget(&mw);
        frame->set_layout(new VBoxLayout);
        mw.get_layout()->add_widget(frame);

        // Set a title so we can see which is which
        frame->set_title("Frame " + std::to_string(i));
    }

    mw.show();
}
