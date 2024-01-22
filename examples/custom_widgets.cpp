#include <notcute/main_window.hpp>
#include <notcute/layout.hpp>
#include <notcute/list_widget.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/text_widget.hpp>

using namespace notcute;

// This example will encapsulate the list example into
// a reusable widget

// We will override FrameWidget and encapsulate a ui layout
class CustomWidget : public FrameWidget {
public:
    CustomWidget(Widget* parent = nullptr)
        :FrameWidget(parent)
    {
        // Setting the layout could be the widgets job or whatever is creating the widgets
        // job. For simplicity lets just give it a layout here.
        set_layout(new HBoxLayout);

        ui.list = new ListWidget(this);
        ui.list->set_title("Up/Down or j/k to scroll. Enter to select an item");

        for (int i = 0; i < 10; ++i) {
            ui.list->add_text("LIST ITEM: " + std::to_string(i));
        }

        ui.selected_items = new FrameWidget(this);
        ui.selected_items->set_layout(new VBoxLayout);

        ui.list->item_selected.connect([this](ListItem* item){
                std::string str = item->to_string();
                Label* label = new Label(str, ui.selected_items);
                ui.selected_items->get_layout()->add_widget(label);
                label->get_layout()->set_margins_ltrb(1,0,1,0);
            });

        get_layout()->add_widget(ui.list);
        get_layout()->add_widget(ui.selected_items);
    }

    // I recommend organizing the ui like so
    struct ui{
        ListWidget* list;
        Widget*     selected_items;
    }ui;
};

int main() {

    // MainWindow will automatically fill the terminal screen
    MainWindow mw;

    // Lets add two CustomWidgets just to show off reusing
    // an encapsulated UI via widgets
    CustomWidget* wid1 = new CustomWidget(&mw);
    CustomWidget* wid2 = new CustomWidget(&mw);

    mw.get_layout()->add_widget(wid1);
    mw.get_layout()->add_widget(wid2);

    // Lets give focus to the first widgets list.
    // Normally ui would be private but to avoid making a getter it is public
    wid1->ui.list->set_focus();

    mw.show();
}
