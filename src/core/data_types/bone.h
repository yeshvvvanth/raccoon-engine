#pragma once
#include<fstream>
#include"../maths/maths.h"
typedef unsigned char T_BONE ;

struct Bone {
  unsigned char ID;
  bool deform;
  std::string name;
  unsigned char ch_num;
  unsigned char parentID;
  unsigned char* children;
  mat4 transform;
  mat4 offset;
  void read(std::ifstream&);
  ~Bone();
};
