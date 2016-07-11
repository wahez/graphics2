cmake_minimum_required(VERSION 3.0)

project(graphics2)

add_definitions(-std=c++17)
add_executable(graphics2
    graphics.h
    graphics.cc
    image.cc
    main.cc
    svg.cc
    text.cc
)

target_include_directories(graphics2 PRIVATE
    /usr/include/cairomm-1.0
    /usr/lib/x86_64-linux-gnu/cairomm-1.0/include
    /usr/include/cairo
    /usr/include/glib-2.0
    /usr/lib/x86_64-linux-gnu/glib-2.0/include
    /usr/include/pixman-1
    /usr/include/freetype2
    /usr/include/libpng12
    /usr/include/sigc++-2.0
    /usr/lib/x86_64-linux-gnu/sigc++-2.0/include
)
target_link_libraries(graphics2
    cairomm-1.0
    cairo
    sigc-2.0
)
