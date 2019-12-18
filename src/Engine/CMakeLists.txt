
add_executable(raccoon Main_Editor.cpp)
target_link_libraries(raccoon editor glew openal GL)
set_target_properties( raccoon PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Engine )
