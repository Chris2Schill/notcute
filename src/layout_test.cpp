// LAY_IMPLEMENTATION needs to be defined in exactly one .c or .cpp file that
// includes layout.h. All other files should not define it.

#include <notcute/layout.hpp>
#include <notcute/renderer.hpp>
#include <notcute/box.hpp>
#include <notcute/logger.hpp>
#include <notcute/text_widget.hpp>
#include <notcute/frame_widget.hpp>
#include <notcute/list_widget.hpp>
#include <notcute/edittext.hpp>
#include <notcute/main_window.hpp>

using namespace notcute;

void print(std::string name, Rect rect) {
    log_debug(fmt::format("{} = Pos=({},{}),Size=({},{})", name, rect.x(), rect.y(), rect.width(), rect.height()));
};


ncpp::Plane* from_box(Box* box) {
    Rect r = box->get_rect();
    return new ncpp::Plane(r.height(), r.width(), r.y(), r.x());
}

// void fill(ncpp::Plane* plane, std::string c) {
//     plane->erase();
//
//     Rect rect {
//         Point{
//             .x = plane->get_x(),
//             .y = plane->get_y(),
//         },
//         Size {
//             .width = plane->get_dim_x(),
//             .height = plane->get_dim_y(),
//         }
//     };
//
//     for (int i = 0; i < rect.height(); i++) {
//         for (int j = 0; j < rect.width(); j++) {
//             plane->putstr(i, j, c.c_str());
//         }
//     }
// }

void draw(Box* b, ncpp::Plane* p, const std::string& c) {
    Rect r = b->get_rect();
    p->resize(r.height(), r.width());
    p->move(r.y(), r.x());

    fill(p, c);
    p->perimeter_double(0,0,0);
    p->putstr(0,2,"TOPLEFTBABY");
}

int frame_test() {
    notcute::Renderer* renderer = notcute::Renderer::get_instance();
    Size term_size = renderer->get_term_size();
    notcute::Widget* main_window = new FrameWidget();
    main_window->set_layout(new HBoxLayout(term_size.height, term_size.width));

    notcute::TextWidget* text = new notcute::TextWidget("BOOOOOIIII", main_window);
    text->get_layout()->set_behave(LAY_TOP);
    main_window->get_layout()->add_widget(text);
    text->get_layout()->set_margins_ltrb(0,1,0,0);

    main_window->show();
    return 0;
}

int list_test() {
    MainWindow* main_window = new MainWindow;
    main_window->set_name("main window");
    // main_window->get_layout()->set_margins_ltrb(1,1,1,1);

    notcute::Widget* list_frame = new FrameWidget(main_window);
    list_frame->set_layout(new VBoxLayout(10, 20));
    list_frame->set_name("list-frame");
    list_frame->get_layout()->set_behave(LAY_CENTER);
    // list_frame->get_layout()->set_margins_ltrb(1,1,1,1);
    main_window->get_layout()->add_widget(list_frame);


    // notcute::Widget* wid1 = new FrameWidget(list_frame);
    // wid1->set_layout(new VBoxLayout(5,20));
    // wid1->get_layout()->set_behave(LAY_CENTER | LAY_VFILL);
    // wid1->get_layout()->set_margins_ltrb(0,0,0,0);
    // wid1->set_name("wid1");
    // list_frame->get_layout()->add_widget(wid1);

    ListWidget* list = new ListWidget(list_frame);
    list->get_layout()->set_behave(LAY_LEFT | LAY_VCENTER | LAY_VFILL | LAY_HFILL);
    list_frame->get_layout()->add_widget(list);
    list->get_layout()->set_margins_ltrb(1,1,1,1);
    list->add_text("Option1");
    list->add_text("Option2");
    list->add_text("Option3");
    list->add_text("Option4");
    list->add_text("Option5");
    list->add_text("Option6");
    list->add_text("Option7");
    list->add_text("Option8");
    list->add_text("Option9");
    list->add_text("Option10");
    list->add_text("Option11");
    list->add_text("Option12");
    list->set_focus();
    list->add_widget(new TextWidget("FANCYSHMANCE TEXT"));
    // list_frame->get_layout()->add_widget(new TextWidget("FANCYSHMANCE TEXT", main_window));

    // list_frame->get_layout()->add_widget(new TextWidget("FANCYSHMANCE TEXT", list_frame));

    list->item_selected.connect([](ListItem*item){
            bool is_widget = item->get_widget() != nullptr;
            log_debug(fmt::format("item selected: {}, is_widget={}",item->to_string(), is_widget));
        });

    // main_window->get_layout()->run_context();
    // main_window->draw();
    main_window->show();
    return 0;
}

int edittext_test() {
    notcute::Renderer* renderer = notcute::Renderer::get_instance();
    Size term_size = renderer->get_term_size();
    notcute::Widget* main_window = new FrameWidget();
    main_window->set_layout(new VBoxLayout(term_size.height, term_size.width));

    return 0;
}

int layout_test() {
    Widget* main_window = new MainWindow;
    main_window->set_name("main_window");

    // for (int i = 0; i < 2; ++i)
    // {
    //     notcute::TextWidget* text = new notcute::TextWidget("SCHILLDAWG", main_window);
    //     text->set_name("text_widget");
    //     main_window->get_layout()->add_widget(text);
    //     main_window->get_layout()->set_behave(LAY_CENTER);
    // }

    notcute::Widget* wid1 = new FrameWidget(main_window);
    notcute::Widget* wid2 = new FrameWidget(main_window);
    wid1->set_layout(new HBoxLayout(10,10));
    wid2->set_layout(new VBoxLayout(10,10));
    main_window->get_layout()->add_widget(wid1);
    main_window->get_layout()->add_widget(wid2);

    notcute::Widget* wid1_1 = new FrameWidget(wid1);
    notcute::Widget* wid1_2 = new FrameWidget(wid1);
    wid1_1->set_layout(new VBoxLayout(10,30));
    wid1_2->set_layout(new VBoxLayout(10,10));
    wid1->get_layout()->add_widget(wid1_1);
    wid1->get_layout()->add_widget(wid1_2);

    // wid1_1->get_layout()->set_behave(LAY_VFILL);

    notcute::TextWidget* text = new notcute::TextWidget("SCHILLDAWG", wid1_1);
    wid1_1->get_layout()->add_widget(text);
    wid1_1->get_layout()->set_contain(LAY_LAYOUT);
    text->get_layout()->set_margins_ltrb(1,1,0,0);
    text->get_layout()->set_behave(LAY_LEFT | LAY_TOP);

    main_window->show();
    return 0;
}

int main() {
    return frame_test();
    // return edittext_test();
}

// int main2() {
//     // Size term_size = renderer->get_term_size();
//
//     ncpp::NotCurses nc;
//     unsigned rows, cols;
//     nc.get_term_dim(&rows, &cols);
//     Box* root2 = new Box(rows, cols);
//     // Box* root2 = new Box(1280,800);
//     root2->set_contain(LAY_ROW);
//
//     Box* mlist = new Box(20, 60, root2);
//     Box* mcontents = new Box(1,1, root2);
//
//     Box* mlist_i1 = new Box(20, 20, mlist);
//     mlist_i1->set_behave(LAY_FILL);
//     Box* mlist_i2 = new Box(20, 20, mlist);
//     mlist_i2->set_behave(LAY_FILL);
//     Box* mlist_i3 = new Box(20, 20, mlist);
//     mlist_i3->set_behave(LAY_FILL);
//
//
//     mlist->set_behave(LAY_VCENTER | LAY_LEFT | LAY_VFILL);
//     mlist->set_contain(LAY_COLUMN);
//     //
//     // root2->insert(mcontents);
//     mcontents->set_behave(LAY_HFILL | LAY_VFILL);
//
//     root2->run_context();
//
//
//     print("root", root2->get_rect());
//     print("master list", mlist->get_rect());
//     print("contents", mcontents->get_rect());
//     // print("contents", mcontents->get_rect());
//
//
//     ncpp::Plane* mplane = from_box(mlist);
//     ncpp::Plane* cplane = from_box(mcontents);
//     ncpp::Plane* mlist_i1p = from_box(mlist_i1);
//     ncpp::Plane* mlist_i2p = from_box(mlist_i2);
//     ncpp::Plane* mlist_i3p = from_box(mlist_i3);
//
//     draw(mlist, mplane, "m");
//     draw(mcontents, cplane, "z");
//     draw(mlist_i1, mlist_i1p, "a");
//     draw(mlist_i2, mlist_i2p, "b");
//     draw(mlist_i3, mlist_i3p, "c");
//
//     while(true) {
//         nc.render();
//         std::this_thread::sleep_for(std::chrono::milliseconds(16));
//     }
//
//     return 0;
// }
