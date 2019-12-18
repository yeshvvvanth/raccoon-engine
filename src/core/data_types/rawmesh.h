#pragma once
#include<fstream>
#include"armature.h"
#include"texture.h"
#include"../render/transform.h"
#include"shader.h"

#define MESH_TEX 2
#define MESH_RIGGED 4
#define MESH_ANIM 8


class RawMesh{

 public:
  GLuint vao;
  GLuint vert_data,ebo,uv_data,col_data,bone_data;
  unsigned int vs,es;bool animate;
  RawMesh(unsigned char,std::ifstream&,Shader* sh);
  RawMesh();
  ~RawMesh();
  void read(std::ifstream&,Shader* sh);
  void render(Transform&);
  void set_shader(Shader* sh);
  Box box;
  
  static std::vector<RawMesh*> meshes;
  static std::vector<RawMesh*> emeshes;
  Shader* shader;
  unsigned char type;
  std::string name;
  Armature* arm;
  static RawMesh* findRawMesh(std::string tname,bool e);
  
  
  float *vertices,*uvs,*ccs,*bdata;
  unsigned short *indices;
};
