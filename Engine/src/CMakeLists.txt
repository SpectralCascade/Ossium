cmake_minimum_required(VERSION 3.6)

project(MY_APP)

find_library(SDL2 SDL2)

add_library(Ossium STATIC)

target_link_libraries(Ossium SDL2)


