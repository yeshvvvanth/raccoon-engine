#pragma once
#include<stdio.h>
#include"../render/window.h"
#include<GLFW/glfw3.h>
#include<string.h>

#include"input.h"
#include"../render/camera.h"
#include"../render/entity.h"

#include"../utils/data.h"
#include"../utils/g_dir.h"
#include"../utils/fpscounter.h"

#include"../data_types/shader.h"
#include"../UI/textButton.h"
#include"../render/skykbox.h"
#include"../data_types/audio.h"

#include"../render/physics.h"
class Game{
public:
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btSequentialImpulseConstraintSolver* solver;
    btCollisionDispatcher* dispatcher;
 ///////////////////////
  std::string Rpath,Gdir;
  bool running;
  Window* window;
  Data* data;
  Camera *camera;
  Input input;
  FpsCounter* counter;
  static Label* fpsmeter;
  static void resize_window(int w,int h);
   Game();

  void InitFBOs();
  void Start();
  void initPhysics();
  virtual void Init();
  virtual void Render();
  void Cleanup();
  
  ~Game();
 
};

