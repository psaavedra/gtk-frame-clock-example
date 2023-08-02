cmake_minimum_required(VERSION 3.20)
project(GTKFrameClockExample C)
set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)

# Get the current working directory (source directory)
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

# Print the current working directory
message("Source Directory: ${CURRENT_DIR}")

add_executable(gtk-frame-clock-example src/main.c)

target_link_libraries(gtk-frame-clock-example ${GTK3_LIBRARIES})
target_include_directories(gtk-frame-clock-example PUBLIC ${GTK3_INCLUDE_DIRS})
target_compile_options(gtk-frame-clock-example PUBLIC ${GTK3_CFLAGS_OTHER})

include_directories(${WAYLAND_CLIENT_INCLUDE_DIRS})
link_directories(${WAYLAND_CLIENT_LIBRARY_DIRS})
add_definitions(${WAYLAND_CLIENT_CFLAGS_OTHER})

target_link_libraries(gtk-frame-clock-example ${WAYLAND_CLIENT_LIBRARIES})

