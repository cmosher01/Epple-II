cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_FIND_DEBUG_MODE TRUE)

project(epple2)

include(GNUInstallDirs)

add_subdirectory(src)
add_subdirectory(conf)
