#pragma once
#include<main/scene.h>
#include<UI/textfield.h>
#include"selectionmenu.h"

struct Property{
   short h;
   char type;
  virtual void tick()=0;
  virtual void sort(short&)=0;
  Property(short h, char type);
  virtual ~Property();
};
////////////////////////
struct Prop_Vector:public Property{
  Label* label;
  FtextField* buttons[3];
  Prop_Vector(std::string name,vec3 vec);
  virtual~Prop_Vector();
   vec3 toVec();
   void fromVec(vec3 v);
  void tick();
  void sort(short &tH);
};
struct Prop_Vec2:public Property{
  Label* label;
  FtextField* buttons[2];
  Prop_Vec2(std::string name,vec2 vec);
  virtual~Prop_Vec2();
   vec2 toVec();
   void fromVec(vec2 v);
  void tick();
  void sort(short &tH);
};

template<class T>
struct Prop_Select:public Property{
  Label* label;
  SelectionMenu<T> *selMenu;
  Prop_Select(std::string name,std::vector<T*> vec,bool nonone=false);
  ~Prop_Select();
  void tick();
  void sort(short &tH);
};


struct Prop_String:public Property{
  Prop_String(std::string name,std::string str);
  virtual~Prop_String();
  Label* label;
  TextField* field;
  void tick();
  void sort(short &tH);
};


struct Prop_Float:public Property{
  Prop_Float(std::string name,bool b);
  Label* label;
  FtextField* ftext;
  void tick();
  void sort(short &tH);
  virtual~Prop_Float();
};

struct Prop_Bool:public Property{
  Prop_Bool(std::string name,bool b);
  Label* label;
  Button* button;
  void tick();
  void sort(short &tH);
  ~Prop_Bool();
};

struct Prop_Slide:public Property{
  Prop_Slide(std::string name,bool b);
  Label* label;
  Slider* slide;
  void tick();
  void sort(short &tH);
  ~Prop_Slide();
};
