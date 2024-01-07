cmake_minimum_required(VERSION 3.2)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ncqt VERSION 0.1)

if(WIN32 AND NOT MSVC)
    add_compile_options(-Wall -fuse-ld=lld)
elseif(UNIX)
    add_compile_options(-Wall)
endif()

#TODO fix this properly by adding an install to BSI libraries

find_package(Notcurses++ REQUIRED)
find_package(spdlog REQUIRED)

set(SOURCES
    widget.cpp
)

add_library(ncqt STATIC ${SOURCES})
target_link_libraries(ncqt PUBLIC notcurses++ notcurses notcurses-core spdlog::spdlog)
target_link_directories(ncqt PUBLIC /usr/local/lib)
target_include_directories(ncqt PUBLIC include)
