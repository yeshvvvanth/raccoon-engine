#pragma once
#include<fstream>
#include<vector>
#include"shader.h"

struct Texture{
  GLuint texID;
  std::string name;
  unsigned char type;     // ******10
  int width,height;
  unsigned char* image;
 public:
  static std::vector<Texture*> textures;
  static std::vector<Texture*> etextures;
  static Texture* findTexture(std::string tname,bool e);
  Texture(unsigned char t_type,std::ifstream&);
  Texture(GLuint ID=Shader::twhite,short width=1,short height=1);
  void read(std::ifstream&);
  ~Texture();
  void bind(GLuint TextureUnit=GL_TEXTURE0);
};


struct CubeMap{
    GLuint texID;
    std::string name;
    static std::vector<CubeMap*> cubemaps;
    static std::vector<CubeMap*> ecubemaps;
    CubeMap(std::string name,std::ifstream& ifs);
    ~CubeMap();
    void bind(GLuint TextureUnit=GL_TEXTURE2);
};
