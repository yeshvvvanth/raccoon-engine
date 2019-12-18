#pragma once
#include"components.h"

struct Inspector{
  static short H,x,y,w,h,dx,dy;
  static bool scroll,scrolling;
  static ContextMenu *cmenu;
  static Sprite *sp;
  static std::vector<Component*> Components;
  static AddComp *addcomp; 
  static void refresh();
  static SceneComp* scomp;
  static ScrollBar sbar;
  Inspector();
  ~Inspector();
   static void  tick();
   static void sort();
};
