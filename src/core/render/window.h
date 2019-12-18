#pragma once
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef EMSCRIPTEN
    #include<emscripten.h>
    #include<emscripten/html5.h>
    EM_BOOL fullscreenchange_callback(int,const EmscriptenFullscreenChangeEvent*, void*);
#endif

struct Window{
  GLFWwindow* GL_window;
  
  void resize();
  Window(const char*,int,int,int,int,bool);
  ~Window();
  
};
