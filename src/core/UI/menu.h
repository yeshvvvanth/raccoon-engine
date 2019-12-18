#pragma once
#include"textButton.h"

struct ScrollBar{
    ScrollBar();
    void set(float tx,float ty,float tw,float theight);
    float x,y,sbw,pdy,dy,ih,height;
    bool drag,pdrag;vec3 scolour;
    void tick();
};

class Menu{
public:
  Font* font;
  float x,y,width,height,ih,dx,dy,sbw,pad;
  bool visible,islist,scrollable,scroll,rep,heir;
  ScrollBar sbar;
  TextButton* active;
  std::vector<TextButton*> selected;
  std::vector<TextButton*> tbuttons;
  virtual void tick();
  virtual void sort();
  void set(float x,float y);
  virtual void repos(TextButton* tb,char pos);
  virtual TextButton* addButton(std::string text);
  bool hover();
  Menu(float X,float Y,Font* fnt);
  virtual ~Menu();
};


