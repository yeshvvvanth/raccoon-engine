cmake_minimum_required(VERSION 3.0)
file(GLOB LUASOURCES "src/*.c")
add_library(lua STATIC ${LUASOURCES})
target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
