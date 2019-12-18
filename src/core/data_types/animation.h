#pragma once
#include<fstream>
typedef  unsigned char T_BONE;
#include"../maths/maths.h"

struct Key{
  unsigned char frame_num;
  vec3 pos;
  quat rot;
  vec3 scale;
};
struct Pbone {
  unsigned char ID;
  Key *keys;
  ~Pbone();
};

struct Animation {
  unsigned char ID;
  unsigned char n_len;
  std::string name;
  unsigned char fps;
  unsigned short num_frames;
  unsigned short num_keys;
  Pbone* pbones;
  void read(T_BONE,std::ifstream&);
  ~Animation();
  
};
