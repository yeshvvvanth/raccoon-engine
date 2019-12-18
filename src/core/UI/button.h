#pragma once
#include"../render/sprite.h"
#include"../main/input.h"
/*
 *button=0;textbutton=1;Slider=2;ttextfield=3;ftextfield =4;label=5;sprite=6
*/
class Button: public Sprite{
public:
  bool down,pdown,justDown,over,toggle,dragged,isToggle;
  static GLuint uniTint;
  static Button *b1,*b2;
  char type;
  vec3 uColor,aColor;
  Button(float x,float y,float width,float height);
  virtual ~Button();
  virtual void setPos(float X,float Y);
  virtual void draw();
  void tick(bool scale=false);
};



struct Slider: public Button{
    Button *handle;
    Slider(float x,float y,float width,float height);
    ~Slider();
    float value;
    void setPos(float X,float Y);
    void tick();
};
