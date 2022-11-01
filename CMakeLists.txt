cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(epple2)

include(GNUInstallDirs)

add_subdirectory(src)
add_subdirectory(conf)
#add_subdirectory(rom)
