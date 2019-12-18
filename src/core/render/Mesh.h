#pragma once
#include"Material.h"
#include"../data_types/rawmesh.h"
#include"physics.h"
#include<fstream>
struct Renderable{
    vec2 tile,tileoff;
    Texture* texture;
    char type;Box box;
    virtual void tick(Transform& trans,float delta=0)=0;
    virtual ~Renderable();
    char flags;
};
struct Mesh:Renderable{
    RawMesh* rawmesh;
    float smoothness;
    vec3 glow;float ff;int f; // animation frame number and frame fraction
    Mesh(RawMesh* rawmesh,Material* material=0);
    void setAnimation(int num=-1,std::string tname="");
    void tick(Transform& trans,float delta=0);
    unsigned short anum;float animSpeed,transSpeed;
    btCollisionShape* getCollider(char type=0);
};
struct Quad:Renderable{
    char facing;
    Quad();
    short f,sf,ef;float aspeed,timer;
    void tick(Transform& trans,float delta=0);
};
struct Terrain:Renderable{
    RawMesh* raw;
    unsigned int sizex,sizey;
    Shader* shader;
    Texture *textures[3];
    void tick(Transform& trans,float delta=0);
    void reNormals();
    float* terr(unsigned int x,unsigned int y),maxh,minh,scale;
    short* data;unsigned char*tdata;
    Terrain(int sx=128,int sy=128,std::ifstream *ifs=0);
    btHeightfieldTerrainShape* getCollider();
    void write(std::ofstream &ofs);
    void read(std::ifstream *ifs);
    void Refresh();
    ~Terrain();
};
#define TRANSPARENT 1
#define FACE 2
#define DOSHADOW 4
#define DODEPTH 8
#define UNSHADED 16
