
add_executable(layouts layouts.cpp)
target_link_libraries(layouts notcute)

add_executable(list list.cpp)
target_link_libraries(list notcute)

add_executable(custom_widgets custom_widgets.cpp)
target_link_libraries(custom_widgets notcute)

add_executable(focus_graph focus_graph.cpp)
target_link_libraries(focus_graph notcute)

add_executable(scroll_area scroll_area.cpp)
target_link_libraries(scroll_area notcute)

add_executable(nested_focusables nested_focusables.cpp)
target_link_libraries(nested_focusables notcute)
