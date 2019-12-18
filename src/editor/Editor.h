#pragma once
#include<main/game.h>
#include"loader/loader.h"
#include"grid.h"
#include"EditorUI.h"
#include"tinyfiledialogs.h"
class Editor:public Game{
  bool wireframe,play,cam_edit;
public: 
  Editor();
  ~Editor();
  Grid* grid;
  void Reload();
  void Save();
  void Play();
  void Init();
  void Render();
  void reloadAss();
  void handleInput();
  static Button *icon;
  Scene *tactive;
  void editor_camera();
  void e_render(Scene*);
  //Editor UI stuff
  static EditorUI* eui;
  static bool showui;
  static Scene* actScene;
  void loadPrefs(bool startup=true);
  void savePrefs();
};


void Render();
void cleanup();
void startLoop();
