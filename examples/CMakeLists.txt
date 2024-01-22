cmake_minimum_required(VERSION 3.2)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(notcute_examples VERSION 0.1)

macro(build_and_install target)
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} notcute spdlog::spdlog)

    install(TARGETS ${target}
            DESTINATION examples
    )
endmacro()

find_package(notcute REQUIRED)
find_package(spdlog REQUIRED)

build_and_install(layouts)
build_and_install(list)
build_and_install(custom_widgets)
build_and_install(focus_graph)
build_and_install(scroll_area)
build_and_install(nested_focusables)
