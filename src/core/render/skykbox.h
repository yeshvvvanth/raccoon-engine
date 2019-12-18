#pragma once
#include"../data_types/texture.h"
#include"../data_types/shader.h"

struct Skybox{
    GLuint vao;
    Shader* shader;
    Skybox();
    ~Skybox();
    void tick();
    CubeMap* cm;
};
