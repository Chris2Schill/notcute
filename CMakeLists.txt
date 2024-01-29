cmake_minimum_required(VERSION 3.2)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(notcute VERSION 0.1)

if(WIN32 AND NOT MSVC)
    add_compile_options(-Wall -fuse-ld=lld)
elseif(UNIX)
    add_compile_options(-Wall)
endif()

#TODO fix this properly by adding an install to BSI libraries

find_package(Notcurses++ REQUIRED)
find_package(spdlog REQUIRED)

set(SOURCES
    src/widget.cpp
    src/box_layout.cpp
    src/layout.cpp
    src/layout_item.cpp
    src/event_loop.cpp
    src/scroll_area.cpp
    src/frame_widget.cpp
    src/list_widget.cpp
    src/focus_stack.cpp
    src/visual.cpp
)

add_library(notcute STATIC ${SOURCES})
target_link_libraries(notcute PUBLIC notcurses++ notcurses notcurses-core spdlog::spdlog)
target_link_directories(notcute PUBLIC /usr/local/lib)
target_include_directories(notcute PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

add_executable(layout_test src/layout_test.cpp)
target_link_libraries(layout_test PRIVATE notcute)

macro(build_and_install target)
    add_executable(${target} examples/${target}.cpp)
    target_link_libraries(${target} notcute spdlog::spdlog)

    install(TARGETS ${target}
            DESTINATION examples
    )
endmacro()

build_and_install(layouts)
build_and_install(list)
build_and_install(custom_widgets)
build_and_install(focus_graph)
build_and_install(scroll_area)
build_and_install(nested_focusables)


install(DIRECTORY include/
        DESTINATION include
)

install(TARGETS notcute
        EXPORT notcute_cmake_export
        DESTINATION lib
)

install(
    DIRECTORY examples/
    DESTINATION examples/
)

install(EXPORT notcute_cmake_export
    FILE notcuteConfig.cmake
    DESTINATION cmake
)
