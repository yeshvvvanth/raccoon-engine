#include"fpscounter.h"

FpsCounter::FpsCounter()
{
 curr=0;frames=0;lastsec=0;fps=60.0;
 deltaTime=0;
}

void FpsCounter::update(){
 
  curr=glfwGetTime();
  deltaTime=curr-lastsec;
  if(deltaTime>=1.0f){
   fps=frames*1.0f/(deltaTime);
   frames=0;lastsec=curr;
  }
  frames++;
}
