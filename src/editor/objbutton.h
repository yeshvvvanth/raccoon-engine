#pragma once
#include<UI/textButton.h>
#include<render/entity.h>
template <class T>
struct ObjButton:public TextButton{
  T* object;
  bool expand;
  ObjButton(float x, float y, std::string ltex,T* obj, Font* font);
  void tick();
};

