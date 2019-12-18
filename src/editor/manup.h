#pragma once 
#include<render/Mesh.h>
#include<maths/vec3.h>
#include<iostream>
struct Manupulator{
  Manupulator();
  RawMesh* trans,*rot,*scale,*manup;
  vec3 pcpt[3];
  int a=3;
  void tick();
  bool local;
  bool  rayAxis(vec3 ray,vec3 cp,float r,vec3 axis,vec3& ip);  
  static vec3 scup,scright;
};
