add_definitions(-DGLEW_STATIC)
file(GLOB CSOURCES "*/*.cpp" "*/*.h")
add_library(core STATIC ${CSOURCES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core freetype bullet lua)

# remove openal dependancy as soon as possible !!!!
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_library(openal STATIC IMPORTED)
    set_property(TARGET openal PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/openal32.lib")
endif()

find_package(OpenGL REQUIRED)
# find_package(OpenAL REQUIRED)
target_link_libraries(core glfw glew ${OPENGL_LIBRARIES} openal)
