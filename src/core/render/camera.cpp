#include"camera.h"
#include"../utils/data.h"

Camera* Camera::active_camera=0;
Camera* Camera::default_camera=0;
Data* Camera::data=0;
Camera::Camera(bool orthographic):fov(60.0),size(10.0),aspect_ratio(16.0/9.0),near(0.01),far(1000.0)
{
 if(active_camera==0||active_camera==default_camera){
   active_camera=this;
   if(Scene::activeScene&&!Scene::activeScene->camera)
      Scene::activeScene->camera=this;
}
 ortho=orthographic;
  
 pos=vec3(1.0f,5.0f,10.0f);
 dir=vec3(1.0f,1.0f,1.0f);
 up =vec3(0.0f, 1.0f, 0.0f);

 set();
 
 if(data)update(0);
 
}

void Camera::update(Transform *trans)
{
  ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
//   alListener3f(AL_POSITION, pos.x, pos.y,pos.z);
  // check for errors
//   alListener3f(AL_VELOCITY, 0, 0, 0);
  // check for errors
//   alListenerfv(AL_ORIENTATION, listenerOri);
  
  if(trans){
    pos=trans->position;
    rot=trans->rotation.inverse();
    dir=quat::Rotate(trans->rotation,-vec3::Zaxis());

    view=mat4(1.0);
    
    view[12]=-pos.x;
    view[13]=-pos.y;
    view[14]=-pos.z;
    
    view=mat4::Rotate(rot)*view;
  }
  proview=proj*view;
//    view = mat4::LookAt(pos,dir,up);
   
 /// ////////////////////////////
  for(int i=0;i<data->shaders.size();i++){
    data->shaders[i]->use();
    
    glUniform3fv(data->shaders[i]->uniCam, 1, pos.elements);
    
  }
 
}
void Camera::set()
{
    if(ortho)
        proj=mat4::Orthographic(-aspect_ratio*size,aspect_ratio*size,-size,size,near,far);
    else
        proj=mat4::Perspective((fov)*M_PI/180.0,aspect_ratio,near,far);
    //proj=mat4::Perspective((60.0f)*M_PI/180.0f, 16.0f/9.0f, 0.0f, 1000.0f);
}
vec3 Camera::getCamRay(bool notmouse,float px,float py)
{
    vec3 cd=dir;
    if(!ortho){
        vec2 mp=Shader::scrMousePos();
        if(notmouse)mp=vec2(px,py);
        mp.x/=Shader::sc_res.x/2.0;
        mp.y/=Shader::sc_res.y/2.0;

        vec4 mr=vec4(mp.x,mp.y,-1.0,1.0);
            
        mr=mat4::Inverse(proj)*mr;
        mr.z=-1.0;mr.w=0.0f;
        
        mr= mat4::Inverse(view)*mr;
        cd=vec3(mr.elements).Normalize();
    }
    return cd;
}

vec3 Camera::getCamPos()
{
    vec3 cp=pos;
    if(ortho){
        vec2 mp=Shader::scrMousePos();
        mp.x/=Shader::sc_res.x/2.0;
        mp.y/=Shader::sc_res.y/2.0;
        vec3 cd=dir;vec3 up=vec3(0.0,1.0,0.0);
        vec3 right=cd.Cross(up).Normalize();up=right.Cross(cd);
        cp+=right*aspect_ratio*size*mp.x;
        cp+=up*size*mp.y;
    }
    return cp;
}


Camera::~Camera()
{
      if(active_camera==this ){active_camera=default_camera;}
}
