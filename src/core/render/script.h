#pragma once
#include<stdio.h>
#include<string>
#include<vector>
#include<lua.hpp>
#include"../maths/maths.h"
struct Entity;
class Camera;

struct ScriptProp{
  char type;bool pub;int index;
  std::string name;
  virtual void read(std::string *name)=0;
  virtual void update(std::string *name)=0;
  virtual ScriptProp* clone()=0;
  virtual ~ScriptProp();
};

struct SFloat:public ScriptProp{
  float value;
  SFloat(std::string name,bool tpub=true);
  void read(std::string *name);
  void update(std::string *name);
  ScriptProp* clone();
};

struct Sbool:public ScriptProp{
  bool value;
  Sbool(std::string name,bool tpub=true);
  void read(std::string *name);
  void update(std::string *name);
  ScriptProp* clone();
};

struct Sstring:public ScriptProp{
  std::string value;
  Sstring(std::string name,bool tpub=true);
  void read(std::string *name);
  void update(std::string *name);
  ScriptProp* clone();
};

struct SQuat:public ScriptProp{
  quat value;
  SQuat(std::string name,bool tpub=true);
  void read(std::string *name);
  void update(std::string *name);
  ScriptProp* clone();
};


struct SVec3:public ScriptProp{
  vec3 value;
  SVec3(std::string name,bool tpub=true);
  void read(std::string *name);
  void update(std::string *name);
  ScriptProp* clone();
};

struct STable:public ScriptProp{
  std::vector<ScriptProp*> tprops;
  STable(std::string name);
  void read(std::string *table);
  void update(std::string *table);
  STable* clone();
  ~STable();
};


class Script{
public:
  std::string name;
  Entity* entity;
  static lua_State *L;
  static bool success;
  Script();
  Script(std::string scname);
  ~Script();
  void Init();
  void Update(char type=0);
  void tick();
  Script* clone();
  void loadScriptProps();
  std::vector<ScriptProp*> props,p_props;
  static std::vector<Script*> scripts;
  static std::vector<std::string> errors;
  static void registerScripts();
  static void begin();
  static void end();
  static void read(std::ifstream &ifs,bool eda,char* name);
  static void setAEntity(Entity* ent);
  static Script* active;
  static Entity* aentity;
  static std::string source;
  ScriptProp* findProp(std::string sname);
};

void registerClasses();

vec3 getvec3(lua_State* L,int index);
quat getquat(lua_State* L,int index);
void setvec3(lua_State* L,vec3& vec);
void setquat(lua_State* L,quat& quat);
