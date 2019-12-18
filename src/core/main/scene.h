#pragma once
#include<vector>
#include<string>
#include<fstream>
#include"../render/entity.h"
#include"../UI/textfield.h"
class Data;
class Scene{
public:
  Data* data;
  static bool play,reload;
  static Scene* activeScene,*common,*next,*prefabs;
  static std::vector<Scene*> scenes;
  static std::vector<Scene*> ascenes;
  static Scene def_scene;
  Scene(std::string tname="new_scene");
  Scene(Data*);
  ~Scene();
  Camera* camera;
  Scene* swap;
  Scene* Play();
  std::string name;
  std::vector<Entity*> entities,dentities,rentities,waypoints;
  void render();
  void tick();
  void refresh();
  void navigate();
  void bakeNavigation();
  void read(std::ifstream& ifs);
  void write(std::string Gdir);
  bool getPath(vec3 v1,vec3 v2,std::vector<vec3>&points);
  void setActive();
  std::string newname(std::string tname);
  static Scene* findScene(std::string tname);
  static Entity* PickEntity();
  static bool setScene(char type,int num,bool treload,std::string name);
private:
};

