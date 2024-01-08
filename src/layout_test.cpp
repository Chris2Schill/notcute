#pragma once

#include <fcntl.h>
#include <ncpp/NotCurses.hh>
#include <ncpp/Pile.hh>

#include <notcute/box_layout.hpp>
#include <notcute/renderer.hpp>
#include <notcute/text_widget.hpp>
#define LAY_IMPLEMENTATION
#include <notcute/layout2.hpp>

namespace notcute {

inline Widget* create_widget_row(Widget* p) {

    Renderer* renderer = Renderer::get_instance();

    auto l = new BoxLayout();

    Widget* w = new Widget(p);
    w->set_name("row widget container");
    // w->reparent(p);
    // p->get_layout()->add_widget(w);

    w->set_layout(l);
    // static bool first = true;
    // if (first) {
    //     first = false;
    //     for (int i = 0; i < 1; ++i) {
    //
    //         auto tw = new TextWidget("AAAAAAAAAAAAAAAAAAA|", w);
    //         tw->set_name("text_widget");
    //         w->get_layout()->add_widget(tw);
    //     }
    // }
    // else
    // {
    //     for (int i = 0; i < 1; ++i) {
    //
    //         auto tw = new TextWidget("BBBBBBBBBBBBBBBBBB|", w);
    //         tw->set_name("text_widget");
    //         w->get_layout()->add_widget(tw);
    //     }
    // }
    // auto rect = w->get_geometry();
    // rect.m_size.height = 72/2;
    // w->set_geometry(rect);
    // w->get_layout()->set_geometry(
    //         Rect{w->get_layout()->get_geometry().pos(),
    //         Size{3,4}});

    // log_debug(fmt::format("            {}.Geo={}",
    //     w->get_name(),
    //     w->get_contents_rect().to_string()
    // ));
    // w->get_layout()->set_alignment(Center);
    return w;
}


inline Widget* create_blank_widget() {
    Widget* w = new Widget;
    w->set_layout(new BoxLayout);
    return  w;
}

inline void run_layout_test() {

    Renderer* renderer = Renderer::get_instance();

    Widget* main_window = new Widget();
    main_window->set_name("main window");

    main_window->set_layout(new BoxLayout(main_window));
    main_window->get_layout()->debug_name = "v1";

    Widget* b1 = create_blank_widget();
    b1->set_name("b1");
    b1->get_layout()->debug_name = "b1layout";
    main_window->get_layout()->add_widget(b1);

    Widget* b2 = create_blank_widget();
    b2->set_name("b2");
    b2->get_layout()->debug_name = "b2layout";
    main_window->get_layout()->add_widget(b2);

    EventLoop::get_instance()->process_events();


    log_debug("TEST-----------------");
    log_debug(fmt::format("TEST{}.Geo={}",
                           b1->get_name(),
                           b1->get_geometry().to_string()));

    log_debug(fmt::format("TEST{}.Geo={}",
                           b1->get_layout()->debug_name,
                           b1->get_layout()->get_geometry().to_string()));
    log_debug("TEST-----------------");
    log_debug(fmt::format("TEST{}.Geo={}",
                           b2->get_name(),
                           b2->get_geometry().to_string()));

    log_debug(fmt::format("TEST{}.Geo={}",
                           b2->get_layout()->debug_name,
                           b2->get_layout()->get_geometry().to_string()));

    main_window->show();
    renderer->get_nc()->get(true);
}

inline void run_layout_test2() {

    Renderer* renderer = Renderer::get_instance();

    Widget* main_window = new Widget();
    main_window->set_name("layout_test2");
    // main_window->set_fill("1");

    main_window->set_layout(new BoxLayout(main_window));
    main_window->get_layout()->debug_name = "v1";

    auto row1 = create_widget_row(main_window);
    row1->get_layout()->debug_name = "v2.1";
    row1->set_name("row1");
    // row1->set_fill("2");

    auto row2 = create_widget_row(main_window);
    row2->get_layout()->debug_name = "v2.2";
    // row2->set_fill("3");

    Widget* row1_1 = new Widget();
    row1_1->set_layout(new BoxLayout);
    row1_1->set_name("row1_1");
    // row1_1->set_fill("4");
    row1->get_layout()->add_widget(row1_1);


    Widget* row1_2 = new Widget();
    row1_2->set_layout(new BoxLayout);
    row1_2->set_name("row1_2");
    // row1_2->set_fill("5");
    row1->get_layout()->add_widget(row1_2);

    Widget* row1_1_1 = new Widget();
    row1_1_1->set_layout(new BoxLayout);
    row1_1_1->set_name("row1_1_1");
    // row1_2->set_fill("5");
    row1_1->get_layout()->add_widget(row1_1_1);
    // row1_1->set_fill("1");

    // Widget* row2_1 = new Widget(row1);
    // row2_1->set_layout(new BoxLayout);
    // row2_1->set_name("row2_1");


    // row2->get_layout()->add_widget(row2_1);

    main_window->get_layout()->add_widget(row1);
    main_window->get_layout()->add_widget(row2);

    log_debug(fmt::format("TEST{}.Geo={}",
                           main_window->get_name(),
                           main_window->get_geometry().to_string()));

    log_debug(fmt::format("TEST{}.Geo={}",
                           main_window->get_layout()->debug_name,
                           main_window->get_layout()->get_geometry().to_string()));

    // log_debug(fmt::format("{}.Geo={}",
    //     main_window->get_layout()->get_items()[0]->get_widget()->get_name(),
    //     main_window->get_layout()->get_items()[0]->get_widget()->get_contents_rect().to_string()
    // ));
    //
    // log_debug(fmt::format("{}.Geo={}",
    //     main_window->get_layout()->get_items()[1]->get_widget()->get_name(),
    //     main_window->get_layout()->get_items()[1]->get_widget()->get_contents_rect().to_string()
    // ));
    //
    // main_window->get_layout()->add_widget(new TextWidget("BOIIIII|", main_window));
    // main_window->get_layout()->add_widget(new TextWidget("BOIIIII|", main_window));
    // main_window->get_layout()->add_widget(new TextWidget("BOIIIII|", main_window));
    // main_window->get_layout()->add_widget(new TextWidget("BOIIIII|", main_window));
    // main_window->get_layout()->add_widget(row2);
    // main_window->get_layout()->set_geometry(Rect{Point{0,0},renderer->get_term_size()});

    // main_window->get_layout()->set_geometry(
    //         main_window->get_layout()->get_geometry());

    main_window->show();
    renderer->get_nc()->get(true);
}


void fill_test() {
    Renderer* renderer = Renderer::get_instance();

    Widget* main_window = new Widget();
    main_window->set_name("fill_test");

    main_window->set_layout(new BoxLayout(main_window));
    main_window->get_layout()->debug_name = "v1";

    main_window->set_fill("z");
    main_window->show();
}

}

int main() {
    notcute::run_layout_test2();
}

