#pragma once
#include"entitymenu.h"

struct Ewindow{
     Button *bar,*close;
     float x,y,w,h,ih;
     std::vector<Menu*> menus;
     std::vector<TextButton*> buttons;
     std::vector<vec2> dm;
     std::vector<vec2> db;
     void addButton(float tx,float ty,TextButton* button);
     void addMenu(float tx,float ty,float tw,float th,Menu* menu);
     void tick();
     void sort();
     Ewindow(float x,float y,float w,float h);
     ~Ewindow();
};

 struct Console{
   Label* error;
   ScrollBar sbar;
   short lines;
   void tick();
   Console();
   ~Console();
 };
 
 struct EditorUI{
    TextButton *file,*edit,*view,*window,*help,*mbutts[5];
    Menu *fimenu,*edmenu,*vimenu,*activeMenu;
    TextButton *slb=0,*play,*reload,*newproj,*open,*prefab,*common;float tbh;
    Ewindow *scnwindow=0,*nscnwindow;static Label* actile;
    static Ewindow *awindow;
    EditorUI();
    ~EditorUI();
    void resize();
    void update();
};
///////////////////////////////

