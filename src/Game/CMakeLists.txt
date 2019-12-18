

add_executable(Game Main_Game.cpp)
target_link_libraries(Game core)
set(GDIR linux)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(GDIR windows)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")   #  MacoOS... for the lolz
    set(GDIR mac)
endif()

set_target_properties( Game PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Engine/bin/${GDIR} )
