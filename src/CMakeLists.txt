
add_subdirectory(thirdparty)

if(MAKE_EDITOR)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-g")
    add_subdirectory(core)
    add_subdirectory(editor)
    add_subdirectory(Engine)
else()
    add_subdirectory(core)
    add_subdirectory(Game)
endif()
    
