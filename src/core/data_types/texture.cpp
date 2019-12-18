#include"texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
std::vector<Texture*> Texture::textures;
std::vector<Texture*> Texture::etextures;
std::vector<CubeMap*> CubeMap::cubemaps;
std::vector<CubeMap*> CubeMap::ecubemaps;

Texture::Texture(unsigned char t_type,std::ifstream& ifs):type(t_type)
{
  read(ifs);
}
Texture::Texture(GLuint ID,short width, short height):width(width),height(height)
{
    texID=ID;
}

void Texture::read(std::ifstream& ifs)
{
  unsigned int len=0;unsigned char *mem=0; 
    
    ifs.read((char*)&len,4);
    mem=new unsigned char[len];
    ifs.read((char*)mem,len);
    
    image=stbi_load_from_memory(mem,len, &width, &height, 0,STBI_rgb_alpha);
    
    delete [] mem;

  /// load image to texture here
//   if(image!=NULL)printf("image loaded\n");
  glGenTextures(1,&texID);
  glBindTexture(GL_TEXTURE_2D,texID);

  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

//   printf("------------Texture  %d %d gluint=%d\n",width,height,texID);
  glBindTexture(GL_TEXTURE_2D,0);

  STBI_FREE(image);
//   delete[] image;
}

Texture::~Texture()
{       
   glDeleteTextures(1, &texID);
}
void Texture::bind(GLuint TextureUnit)
{
  glActiveTexture(TextureUnit);  
  glBindTexture(GL_TEXTURE_2D,texID);
}
/// ///////////////////////////////////////////

CubeMap::CubeMap(std::string name, std::ifstream& ifs):name(name)
{
     unsigned int len=0;unsigned char *mem=0; 
     glGenTextures(1, &texID);
     glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
     int width ,height;
     unsigned char *image=0;
     
     for(int i=0;i<6;i++){
        ifs.read((char*)&len,4);
        mem=new unsigned char[len];
        ifs.read((char*)mem,len);
        image=stbi_load_from_memory(mem,len, &width, &height, 0,STBI_rgb);
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        delete[] mem;
        STBI_FREE(image);
     }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
     
}


CubeMap::~CubeMap()
{
}


void CubeMap::bind(GLuint TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
}

Texture* Texture::findTexture(std::string tname,bool e){
    std::vector<Texture*> *v;
    Texture* t=0;
     if(e){v=&etextures;}
     else {v=&textures;}
    
    for(int i=0;i<(*v).size();i++){
        if(tname==(*v)[i]->name ){t=(*v)[i];break;}
    }
    return t;
}
