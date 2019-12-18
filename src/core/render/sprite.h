#pragma once
#include"../data_types/texture.h"
#include"../data_types/shader.h"
#include"transform.h"
class Sprite{
public:
  static Sprite* sp;
  vec3 color;
  Transform transform;
  static GLuint uniTint;
  float alpha;
  static GLuint vao;
  Sprite();
  Sprite(float x,float y,float width,float height);
  virtual ~Sprite();
  static void Init();static void End();
  static GLuint vert_data,uv_data;
  void resize(float x,float y,float w,float h);
  void move(float x,float y);
  Texture* texture;
  void draw();
  virtual void tick();
  static void render(float x=0,float y=0,float w=30.0,float h=30.0,vec3 color=vec3(0.5),float alpha=1.0,Texture* texture=0);
  static Shader* shader;
};
