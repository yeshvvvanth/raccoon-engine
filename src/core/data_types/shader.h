#pragma once
#include<GL/glew.h>
#include<stdio.h>
#include"../render/transform.h"
#include<vector>
#define LightsPerFrag 5
struct Shader{
  const char *vert,*frag;
public: 
  GLuint PID;
  char* name;
  //unsigned char type;
  static vec2 sc_res,sh_res;
  static vec2 getResolution();
  static float getAspectRatio();
  static vec2 scrMousePos(bool scale=false);
  
  static std::vector<Shader*> shaders;
  static std::vector<Shader*> eshaders;
  
  static Shader *simple,*def,*pervert,*render2D,*cubemap,*fontShader,*terrain;
  static Shader*  findShader(const char* sha_name);
  static GLuint twhite,lvao;
  static void Init();
  static void drawLine(vec3 &sp,vec3 &ep,vec3 colour);
  static void renderLine(vec3& sp, vec3& ep,float th1,float th2,vec4 colour,vec4 colour2);
  static void renderPrim(short type,Transform &trans,vec4 colour);
  GLint uniModel,uniMVP,uniAlpha,uniCam,uniTile,uniTileOff,uniSmooth,uniLightMat;
  GLint uniAmbient,uniLightPos[LightsPerFrag],uniLightCol[LightsPerFrag],
        uniLightType[LightsPerFrag],uniLightRadius[LightsPerFrag],uniLightDir[LightsPerFrag];
  GLuint uniRigged,uniAnim,uniTextured;
  Shader( char* sname,const GLchar*,const GLchar*);
  void use();
  ~Shader();
  
};
static const GLint posAttrib=0,boneAttrib=1,weightAttrib=2,texAttrib=3,colAttrib=4,normAttrib=5,
                   tanattrib=6,bitanattrib=7,tfacattrib=8;

