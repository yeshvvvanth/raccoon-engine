#pragma once
#include"button.h"
#include"../text/font.h"

#define PADX 6.0
class TextButton:public Button{
public:
  float Dim,padX,padY;
  vec3 bgColor,activeColor;
  Label* label;
  bool isLabel,visible;
  void setPos(float X,float Y);
  void setSize(float W,float H);
  void setPos(float X,float Y,float W,float H);
  TextButton(float x, float y,std::string ltex,Font* font=0,bool islabel=false);
  virtual ~TextButton();
 void draw();
};
