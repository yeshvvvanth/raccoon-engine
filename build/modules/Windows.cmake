# i686 or x86_64
set(ARCH i686)
add_definitions(-DGLEW_STATIC)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER ${ARCH}-w64-mingw32-gcc) 
set(CMAKE_CXX_COMPILER ${ARCH}-w64-mingw32-g++) 

set( CMAKE_EXE_LINKER_FLAGS "-static" CACHE STRING "" FORCE)
# set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS}  -static-libgcc " )
# set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  -static-libstdc++ " )
