#include<GL/glew.h>
#include<data_types/shader.h>
#include<maths/maths.h>
#include<render/camera.h>
#include<data_types/texture.h>

class Grid{
public:
    GLuint gvao,uniColor,uniModel;

    Shader* shader;
    Texture* tex;
    
    Grid(Shader* sha);
    ~Grid();
    void init();
    void draw();
    
};
