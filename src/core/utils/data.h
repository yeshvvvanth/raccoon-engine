#pragma once
#include<vector>
#include<fstream>

#include"../data_types/shader.h"
#include"../render/Mesh.h"
#include"../data_types/texture.h"
#include"../render/sprite.h"
#include"../render/camera.h"
#include"../main/scene.h"
#include"../data_types/audio.h"

#define T_MESH    1
#define T_TEXTURE 2
#define T_AUDIO   4
#define T_SCRIPT  8
#define T_FONT    16
#define T_SHADER  32
#define T_SCENE   64

class Data{
public:

  std::vector<Shader*>shaders;
  std::vector<RawMesh*> meshes;

  std::vector<Texture*> textures;
  std::vector<Scene*> scenes;
  unsigned int fpos;
  void reload();
  std::string path;
  
  void loadShaders(std::ifstream&,unsigned short&);
  void loadAssets(std::ifstream&,unsigned short&,bool eda);
  void loadScenes(std::ifstream&,unsigned short&);
  
  RawMesh*    findRawMesh(const char* mesh_name);
  Texture* findTexture(const char* tex_name);
  
  Data(std::string);
  ~Data();
};
