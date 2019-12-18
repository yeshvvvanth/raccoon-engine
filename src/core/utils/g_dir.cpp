#include"g_dir.h"
#include<string.h>
#ifdef __linux__
#include<unistd.h>
#elif _WIN32
#include <windows.h>
#endif

std::string path::GetBasePath(){
    std::string path;
    char buffer[B_SIZE];
    memset(buffer,'\0',B_SIZE);
    #ifdef __linux__
        readlink("/proc/self/exe", buffer, B_SIZE);
    #elif _WIN32
        GetModuleFileName( NULL, buffer, B_SIZE );
    #else
        //only for linux and windows currently
    #endif
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" )+1;
    path= std::string( buffer ).substr( 0, pos);
    return path;
}
void path::clear(char *path){
    delete [] path;
    
}
