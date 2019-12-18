#include<Editor.h>
#include<iostream>
//////////
void* handle;
typedef void reg_t(std::vector<Script*>&);
void reload(bool des);
/////////

Editor* editor;
Input input;

/////////
int main(){
 glfwInit();
   editor=new Editor();
   editor->Start();
   input=editor->input;
     
   printf("Rpath= %s Gdir=%s \n",editor->Rpath.c_str(),editor->Gdir.c_str());
   startLoop();

 cleanup();
 glfwTerminate();
 return 0;
}

void Render(){
  glfwPollEvents();
  
 editor->Render();
 
 Input::reset();
}
void startLoop()
{
#ifdef EMSCRIPTEN
 emscripten_set_main_loop(Render,0,1);
#else
 
  while(editor->running){Render();}
 
#endif
}

void cleanup()
{
  delete editor;
}


