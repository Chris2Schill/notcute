#include <notcute/main_window.hpp>
#include <notcute/layout.hpp>
#include <notcute/list_widget.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/text_widget.hpp>

using namespace notcute;

int main() {

    // MainWindow will automatically fill the terminal screen
    MainWindow mw;
    mw.get_layout()->set_contain(LAY_ROW);

    // List already comes with a VBoxLayout
    ListWidget* list = new ListWidget(&mw);

    // ListWidget by default has FocusPolicy::FOCUS but we have to
    // set it as the one the focused_widget if we want it to
    // receive input events
    list->set_focus();

    // Lets set a title on the list, we will use it for instructions for how to use the list
    list->set_title("Up/Down or j/k to scroll. Enter to select an item");

    // Lets add some text items to the list
    for (int i = 0; i < 10; ++i) {
        list->add_text("LIST ITEM: " + std::to_string(i));
    }

    // Lets connect the list's item_selected signal to
    // add a text widget to a different layout.
    FrameWidget* selected_items = new FrameWidget(&mw);
    selected_items->set_layout(new VBoxLayout);

    list->item_selected.connect([=](ListItem* item){
            std::string str = item->to_string();
            Label* label = new Label(str, selected_items);
            selected_items->get_layout()->add_widget(label);

            // Because we are putting these labels in a frame, we may want
            // to avoid covering that frame. We can do that by setting some margins
            label->get_layout()->set_margins_ltrb(1,0,1,0);
        });

    // Add our widgets to the main window's layout
    mw.get_layout()->add_widget(list);
    mw.get_layout()->add_widget(selected_items);

    mw.show();
}
