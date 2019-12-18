#pragma once
#include"../maths/maths.h"


class Transform{
public:
  vec3 position;
  vec3 scale;
  quat rotation;
  mat4 model;
  static mat4 unit;
  vec3 center;
public:
  Transform();
  void update();
  Transform inverse();
  Transform operator *(Transform tr);
  void multiply(Transform &t);
  void setEuler(float x,float y,float z);
};

struct Box{
    vec3 ll,ul,odx,ody,odz;
    vec3 L,U,dx,dy,dz;
    
    Box();
    Box(vec3 ll,vec3 ul);
    bool IntersectPlane(vec3 point,vec3 normal);
    void getLightBounds(vec3 cp,vec3 cd,float *bounds);
    bool IntersectSphere(vec3 center,float radsq,float &dist);
    bool IntersectRay(vec3 ray,vec3 rpos,vec3& point);
    void tick(Transform &trans);

};
