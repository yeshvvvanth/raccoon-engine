#include<main/game.h>

void Render();
void cleanup();
void startLoop();

Game* game;

int main(){
   glfwInit();
   game=new Game();
   game->Start();
   startLoop();

cleanup();
glfwTerminate();
return 0;
}

void Render(){
 
 game->Render();
 
}
void startLoop()
{
#ifdef EMSCRIPTEN
 emscripten_set_main_loop(Render,720,1);
#else
  while(game->running){Render();}
#endif
}

void cleanup()
{
  delete game;
}
