#include"skykbox.h"
#include"camera.h"
Skybox::Skybox()
{
    float skyboxVertices[] = {
    // Positions          
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
     
    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    
     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    
    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f
    
    };
    
    
    shader=Shader::findShader("cubemap");
    shader->use();
    
    GLuint skyboxVBO;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    cm=CubeMap::ecubemaps[0];
//     glEnableVertexAttribArray(0);
//     glBindVertexArray(0);
    
}
#include"light.h"

void Skybox::tick()
{
    Camera*   camera=Camera::active_camera;
    mat4 mvp=camera->proj,view=camera->view;

    if(Camera::active_camera->ortho){// if active camera is orthogonal
        mvp=Camera::default_camera->proj;
    }

    shader->use();
    cm->bind();
    glBindVertexArray(vao);
    view.elements[12]=0;view.elements[13]=0;view.elements[14]=0;mvp=mvp*view;
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0,36);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

Skybox::~Skybox()
{
    
    
}
