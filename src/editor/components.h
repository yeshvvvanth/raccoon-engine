#pragma once
#include"properties.h"


struct Component{
  short H,type,x,y;
  bool expand,minimized,empty;
  Component();
  virtual ~Component();
  virtual void set()=0;
  virtual void get()=0;
  virtual void tick()=0;
  virtual void sort()=0;
  virtual void remove();
};

//////////////////

struct Transpector:public Component{
  Transpector();
  ~Transpector();
  Prop_Vector* props[3];

  void set();
  void get();
  void tick();
  void sort();
};

struct MeshComp:public Component{
  MeshComp();
  ~MeshComp();
  Label* label;
  Prop_Select<RawMesh> *meshSelect;
   Prop_Select<Texture> *textureSelect,*tselects[3];
   Prop_Select<Animation> *animSelect;
   Prop_Vec2 *tile,*tileoff,*tsize; 
   Prop_Float *smoothness,*aspeed,*sframe,*eframe,*tscale;
   Prop_Bool *face,*transparent,*doshadow,*depthtest,*unshaded;
   vec2 ptsize;float ptscale;
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};

struct EntComp:public Component{
  EntComp();
  ~EntComp();
  Prop_String *entname;
  Button* enable; 
  void set();
  void get();
  void tick();
  void sort();
};

struct ScriptComp:public Component{
  ScriptComp();
  ~ScriptComp();
  Label* scriptName;
  std::vector<Property*> props;
  std::vector<ScriptProp*> scrprops;
  
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};

struct SceneComp:public Component{
  SceneComp();
  ~SceneComp();
  SelectionMenu<Scene> *sceneSelect;
  
  void set();
  void get();
  void tick();
  void sort();
  
};



struct camComp:public Component{
  camComp();
  ~camComp();
   Label* title;
   Prop_Float *near,*far,*aspect_ratio,*fov,*size;
   Prop_Bool *isortho; bool portho;
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};

struct LightComp:public Component{
  LightComp();
  ~LightComp();
   Label* title;
   Prop_Vector *colour,*direction;
   Prop_Float *radius,*intensity;
   Prop_Bool* shadow; 
   
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};

struct SoundComp:public Component{
  SoundComp();
  ~SoundComp();
   Label* title;
   Prop_Float *gain,*pitch;
   Prop_Bool *positional,*loop,*playonawake;
   Prop_Select<Audio> *audio;
  
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};




struct UIComp:public Component{
  UIComp();
  ~UIComp();
   Label* title;
   Prop_Vector *bgcolour,*tcolour;
   Prop_Float *fx,*fy,*fw,*fh,*fvalue,*fontsize,*alpha;
   Prop_String *utext;
   Prop_Select<Texture> *bgselect,*handleselect;
   Prop_Select<Font>* font;
   Prop_Vec2 *pad;
   float pfsize;vec2 ppad;
   std::string ptext,pfont;
  
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};

struct PhysicsComp:public Component{
  PhysicsComp();
  ~PhysicsComp();
   Label* title;
   Prop_Vector *offset,*roffset,*boxSize,*afac,*lifac;
   Prop_Float *radius,*height,*mass,*friction,*restitution;
   Prop_Bool *trigger,*kinematic,*istatic,*nosleep;
    float pradius,pheight,pmass,pfriction,prestitution;
    vec3 poffset,proffset,pboxsize,pafac,plifac,pscale;
    bool ptrigger,pkinematic,pstatic,psleep;
  
  void set();
  void get();
  void tick();
  void sort();
  void remove();
};

struct WPcomp:public Component{
  WPcomp();
  ~WPcomp();
  Label* title;
  void set();
  void get();
  void tick();
  void sort();
  void remove();
    
};

struct GMcomp:public Component{
    Label* title;
    Prop_Float *size;
    Prop_Vector* dim;
    GMcomp();
    ~GMcomp();
    void set();
    void get();
    void tick();
    void sort();
    void remove();
    int psize;vec3 pdim;
};

struct AddComp:public Component{
  AddComp();
  ~AddComp();
  TextButton* tbutton;
  Menu* cmenu;
  Menu* active;
  
  Menu *meshMenu,*scriptMenu,*lightMenu,*soundMenu,*UIMenu,*physicsMenu;
  static LightComp* lc;
  static MeshComp *mc;
  static SoundComp *ac;
  static camComp *cc;
  static UIComp *uc;
  static PhysicsComp *pc;
  static ScriptComp *sc;
  
  void set();
  void get();
  void tick();
  void sort();
  
};
