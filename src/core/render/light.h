#pragma once
#include"../maths/vec3.h"
#include"../data_types/shader.h"
#include<vector>

#define T_DIRECT 0
#define T_POINT 1
#define T_SPOTLIGHT 2
#define NUM_PLIGHTS 4
struct Light{
  static short num,dirlradius;
  static GLuint ShadowBuffer,depthMap,shadowCubeFbo,m_shadowMap;
  char  type;
  float intensity;
  bool shadow;
  vec3 colour;
  vec3 position;
  vec3 direction;
  float radius;
  float angle;
  static std::vector<Light*> lights;
  void doShadow(vec3 pos,float* bounds);
  void doCubeShadow();
  void update(Shader *shader,char type=1);
  static void setAmbient(Shader*,vec3);
  static void reset(Shader *shader);
  Light(char type);
  ~Light();
};


