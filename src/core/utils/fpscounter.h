#pragma once
#include<GLFW/glfw3.h>
#include<stdio.h>

class FpsCounter{
 unsigned int frames;
 double curr,lastsec;
public:
 float fps,deltaTime;
 void update();
 FpsCounter();
};
