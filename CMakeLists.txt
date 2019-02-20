cmake_minimum_required(VERSION 3.6)

project(sexy-pack C)

include_directories(include)
add_library(sexy-pack src/sexy-pack.c)
set_property(TARGET sexy-pack PROPERTY C_STANDARD 99)

add_library(sexy-pack-pic OBJECT src/sexy-pack.c)
set_target_properties(sexy-pack-pic PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE TRUE)
