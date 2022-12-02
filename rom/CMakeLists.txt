cmake_minimum_required(VERSION 3.22.1)
project(epple2roms)
add_custom_target(roms ALL COMMAND make WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
