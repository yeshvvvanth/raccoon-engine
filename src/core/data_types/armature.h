#pragma once
#include"bone.h"
#include"animation.h"
#include<vector>
#include"shader.h"
#define maxbones 24         // 24 for  webgl 1.0 // 52 for webgl 2.0
#define tmaxbones 72
struct Armature{
  T_BONE num_bones;
 public:
  bool isAnimated,transition;
  unsigned char num_an,active_anim,past_anim;
  Bone* bones;
  float matrices[tmaxbones*16],transforms[tmaxbones*16];float trf,aspeed,tspeed;int tf;
  std::vector<Animation*> animations;
  void read(std::ifstream&);
  void animate(GLuint shader_ID,int &f,float &ff,bool go=true);  ////////////
  void rig(GLuint shader_ID);
  Armature(std::ifstream&,bool isanim);
  ~Armature();
};
