
file(GLOB ESOURCES "*.cpp" "loader/*.cpp")
add_library(editor STATIC ${ESOURCES})
target_include_directories(editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editor core)
