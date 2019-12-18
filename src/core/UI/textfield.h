#pragma once
#include"textButton.h"

struct Ibeam{
  GLuint vao;
  mat4 model;
  vec3 color;
  float x,y,height;
  Ibeam();
  void draw(float x,float y);
};

class TextField:public TextButton{
public:
  static Ibeam* ibeam;
  float width;
  bool typing;
  TextField(float x, float y,float width, std::string ltex, Font* font);
  void draw();
  void check();
};

class FtextField:public TextField{
public:
  TextButton *tb;
  char clamp;
  bool stop;
  float value;
  void draw();
  void setValue(float v);
  void setPos(float X,float Y);
  FtextField(float x, float y,float width,float val,std::string ltex,Font* fnt);
  ~FtextField();
};
