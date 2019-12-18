#pragma once
#include"transform.h"
#include"../data_types/shader.h"
class Data;
class Camera{
public:
 vec3 forward,strafe;
 static Camera* active_camera;
 static Camera* default_camera;
 static Data* data;
 float fov,size,aspect_ratio,near,far;
 mat4 proj,view,proview;
 vec3 pos;
 vec3 dir;
 vec3 up;
 quat rot;
 bool ortho;
public:
 Camera(bool orthographic=false);
 ~Camera();
 void set();
 void update(Transform *trans);
 vec3 getCamPos();
 vec3 getCamRay(bool notmouse=false,float px=0,float py=0);
};
