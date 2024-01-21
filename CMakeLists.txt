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
)

add_library(notcute STATIC ${SOURCES})
target_link_libraries(notcute PUBLIC notcurses++ notcurses notcurses-core spdlog::spdlog)
target_link_directories(notcute PUBLIC /usr/local/lib)
target_include_directories(notcute PUBLIC include)

# add_executable(layout_test src/layout_test.cpp)
# target_link_libraries(layout_test PRIVATE notcute)

add_executable(layout_test src/layout_test.cpp)
target_link_libraries(layout_test PRIVATE notcute)
