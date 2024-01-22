#include <notcute/main_window.hpp>
#include <notcute/scroll_area.hpp>
#include <notcute/list_widget.hpp>

using namespace notcute;

int main() {

    MainWindow mw;

    // Lets Start with a scroll area. You can start with the content widget
    // and pass it to the ScrollArea as the first param instead if you like.
    //
    // The ScrollArea will effectivly act as a "window"
    // to the content that is behind it.
    ScrollArea* sq = new notcute::ScrollArea(nullptr, &mw);
    sq->set_layout(new VBoxLayout(45,60));
    sq->set_title("Scroll Area: Up/Down/Left/Right or h/j/k/l");
    sq->set_frame_fg(notcute::DARK_YELLOW);
    sq->set_frame_title_fg(notcute::BLUE);
    sq->get_layout()->set_behave(LAY_CENTER);


    // Now lets create the content that is actually scrollable.
    // For this we will just create a vertical layout that
    // contains a few FrameWidgets that each have their own ListWidget
    Widget* content = new Widget(sq);
    content->set_layout(new VBoxLayout(75,100));
    for (int i = 0; i < 3; ++i) {
        auto frame = new FrameWidget(content);
        frame->set_layout(new VBoxLayout(20,20));

        auto list = new ListWidget(frame);
        list->set_title("A really really long title so that you can tell it is scrolling horizontally");
        list->set_frame_title_fg(LIGHT_GREEN);
        // list->get_layout()->set_margins_ltrb(1,1,1,1);
        for (int i = 0; i < 10; ++i) {
            list->add_text("BOIII"+std::to_string(i));
        }
        frame->get_layout()->add_widget(list);

        content->get_layout()->add_widget(frame);
    }

    // Set the content of the scroll area and add it to the main window.
    // The content can by any widget.
    sq->set_content(content);
    mw.get_layout()->add_widget(sq);

    // Scroll area comes with basic controls for scrolling the content.
    // However a real application may want to utilize a focus graph 
    // and an orchestrator to provide fine-grained controls.
    // Subclasses of ScrollArea can override on_keyboard_event()
    // to do their own logic, or ScrollArea::scroll_vertically() and
    // ScrollArea::scroll_horizontally() can be used to command a ScrollArea
    // to scroll.
    sq->set_focus();

    mw.show();
}
