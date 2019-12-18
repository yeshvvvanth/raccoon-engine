#pragma once
#include<UI/menu.h>
#include"objbutton.h"

template<class T>
struct SelectionMenu:public Menu{
   SelectionMenu(float X, float Y, Font* fnt,std::vector<T*> vec,bool highlight=true);
   T *activeObj;
   bool highlightactive;
   TextButton* title;
   TextButton* none;
   ObjButton<T>* addButton(T* obj);
   void tick();
   void setActive(std::string name);
   ~SelectionMenu();
};

struct ContextMenu:public Menu{
  ContextMenu(float X, float Y, Font* fnt);
  void tick();
};
