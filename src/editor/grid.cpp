#include"grid.h"
#include<data_types/texture.h>

Grid::Grid(Shader* sha){
  shader=sha;
  tex=Texture::findTexture("grid.png",1);
  init();
}
Grid::~Grid(){
    
  
}
  mat4 model;
void Grid::init(){
  
    float s=50000.0f;
    float r=1000.0f;
 float verts[]={
   -s,0.0, s,0.0,r,
    s,0.0,-s,r,0.0,
   -s,0.0,-s,0.0,0.0,
    
    s,0.0,-s,r,0.0,
   -s,0.0, s,0.0,r,
    s,0.0, s,r,r
  };
  
  GLuint vert_data;
  
   glGenVertexArrays(1, &gvao);
   glBindVertexArray(gvao);
   
   shader->use();
  
   
   glGenBuffers(1,&vert_data);
   glBindBuffer(GL_ARRAY_BUFFER,vert_data);
   glBufferData(GL_ARRAY_BUFFER,sizeof(float)*30,verts,GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),0);

   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE,5*sizeof(float), (void*)(3*sizeof(float)));
   
    
}

void Grid::draw(){
    mat4 modl(1.0);
    float fu=4.64;
//     if(Camera::active_camera->pos.y>300){fu=0.08;}
    mat4 mvp=Camera::active_camera->proview;
    
    shader->use();
    glBindVertexArray(gvao);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
    glUniform1f( glGetUniformLocation(shader->PID,"rigged"),0.0f);
    glUniform1f( glGetUniformLocation(shader->PID,"istex"),1.0f);
    
    tex->bind();
     
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));

    glUniform2f(shader->uniTileOff,0.0,0.0);
    glUniform2f(shader->uniTile,fu,fu);
    glDrawArrays(GL_TRIANGLES,0,6);
    /*
    modl.elements[13]=0.1;
    mvp=mvp*modl;
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
    glUniform2f(shader->uniTile,0.32,0.32);
    glDrawArrays(GL_TRIANGLES,0,6);
    */
    
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}
