#pragma once
#include"Mesh.h"
#include"script.h"
#include<vector>
#include"camera.h"
#include"light.h"
#include"../data_types/audio.h"
#include"../UI/button.h"
#include"transform.h"
#include"waypoint.h"

#define NONE 42069
#include"physics.h"

#define NOSHADOW 1
#define NORENDER 2
class Scene;
struct Entity{
  static btDiscreteDynamicsWorld* dynamicsWorld;
  char renderFlags,flags;
//   unsigned short *children;
  std::string name;std::string source;
  Renderable* mesh;
  Camera* camera;
  Light* light;
  Sound* sound;
  Button* button;
  Waypoint* waypoint;
  GridMap* gridmap;
  
  btCollisionShape* collShape;
  MotionState* motionState;
  btRigidBody* rigidBody;
  
  bool enabled;

  Entity* parent;
  std::vector<Entity*> children;   //dont delete at cleanup 
  Script* script;      // delete at cleanup 
  Transform transform,rtrans,ptrans;
  Entity(std::string nam);
  ~Entity();
  static void remove(Entity* dent);
  void unparent();
  void addChild(Entity* child);
  void update();void render();void tick();void init();
  Entity *clone(bool par=true,bool inst=false);
  static Entity* findEntity(std::string tname,char t=0);
  static Entity* findEntity(btCollisionObject* rb);
  static Entity* findEntity(Entity* ent);
  void removeComp(short type);
  bool isEnabled();
  static Entity* isInst(Entity* te);
  void read(std::ifstream& ifs,Scene* scene);
  void write(std::ofstream& ofs,Scene* scene);
};
vec3 toVec3(btVector3);
quat toQuat(btQuaternion);
btVector3 toBVec3(vec3);
btQuaternion toBQuat(quat);
/*
#define isACompound(go) go->rigidBody && go->rigidBody->getCollisionShape()->getShapeType()==COMPOUND_SHAPE_PROXYTYPE \
  && !( go->parent && go->parent->rigidBody && go->rigidBody->getCollisionShape()==go->parent->rigidBody->getCollisionShape())
#define isCompound(go) go->rigidBody && go->rigidBody->getCollisionShape()->getShapeType()==COMPOUND_SHAPE_PROXYTYPE  \
  && ( go->parent && go->parent->rigidBody && go->rigidBody->getCollisionShape()==go->parent->rigidBody->getCollisionShape())
  */
