#include"window.h"
#include"../main/game.h"

Window::Window(const char* name,int px,int py,int width,int height,bool fullScreen)
{       

 #ifndef EMSCRIPTEN
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
 #endif
//     glfwWindowHint(GLFW_SAMPLES,2);
    
    GL_window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(GL_window);
    
    #ifdef EMSCRIPTEN
        EMSCRIPTEN_RESULT ret = emscripten_set_fullscreenchange_callback(0,0,true, fullscreenchange_callback);
        EM_ASM(can.width=960;can.height=540;);
        EM_ASM(gl.viewport(0, 0, 960,540););
        EM_ASM(can.style.width='960px';can.style.height='540px';);
    #else
    glewExperimental=GL_TRUE;
    glewInit();
    
    #endif
    
}

Window::~Window()
{
   glfwDestroyWindow(this->GL_window);
}


#ifdef EMSCRIPTEN

EM_BOOL fullscreenchange_callback(int eventType,const EmscriptenFullscreenChangeEvent *e, void *userData)
{
  if(e->isFullscreen){
      printf("charmander  \n");
      EM_ASM(console.log("screen res="+screen.width+"  "+screen.height));
      
//       EM_ASM(can.width=1600;can.height=900;);
//       EM_ASM(gl.viewport(0, 0,1600,900););
      
      EM_ASM(can.width=1280;can.height=720;);
      EM_ASM(gl.viewport(0, 0,1280,720););
      EM_ASM(can.style.width='100%';can.style.height='100%';);
      
    }
  else{
      printf("bulbasaur  \n");
      EM_ASM(console.log("screen res="+screen.width+"  "+screen.height));

      EM_ASM(can.width=960;can.height=540;);
      EM_ASM(gl.viewport(0, 0, 960,540););
      EM_ASM(can.style.width='960px';can.style.height='540px';);
    }
  float rx=Shader::getResolution().x;
  float ry=Shader::getResolution().y;  
  Game::resize_window(rx,ry);
  return 0;
}

#endif
