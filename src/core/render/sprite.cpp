#include"sprite.h"

Shader* Sprite::shader=0;
GLuint Sprite::uniTint=GL_INVALID_VALUE,Sprite::vao=GL_INVALID_VALUE;
GLuint Sprite::vert_data,Sprite::uv_data;

Sprite::Sprite(){texture=0;color=vec3(1.0f);}

Sprite::Sprite(float x, float y, float width, float height)
{
  transform.position=vec3(x,y,0.0f);
  transform.scale=vec3(width,height,1.0f);
  
  texture=0;
  
  uniTint=glGetUniformLocation(shader->PID,"tint");
  alpha=1.0f;
  color=vec3(1.0);
  transform.center=vec3(width/2,height/2,0.0);
  resize(x,y,width,height);
}
void Sprite::resize(float X, float Y, float w, float h)
{
  transform.position=vec3(X,Y,0.0f);
  transform.scale=vec3(w,h,1.0f);
}
void Sprite::move(float X, float Y)
{
  transform.position=vec3(X,Y,0.0f);
}


void Sprite::Init()
{
   float vertices[18],uvs[12];
   
   uvs[0]=0;uvs[1]=1;
   uvs[2]=1;uvs[3]=1;
   uvs[4]=0;uvs[5]=0;
   
   uvs[6]=1;uvs[7]=1;
   uvs[8]=1;uvs[9]=0;
   uvs[10]=0;uvs[11]=0;
   
   vertices[0]=0; vertices[1]=0; vertices[2]=0;
   vertices[3]=1; vertices[4]=0; vertices[5]=0;
   vertices[6]=0; vertices[7]=1; vertices[8]=0;
   
   vertices[9] =1; vertices[10]=0; vertices[11]=0;
   vertices[12]=1; vertices[13]=1; vertices[14]=0;
   vertices[15]=0; vertices[16]=1; vertices[17]=0;
   
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   
   shader=Shader::render2D;shader->use();
   glUniform1f(shader->uniAlpha,1.0);
   
   glGenBuffers(1,&vert_data);
   glBindBuffer(GL_ARRAY_BUFFER,vert_data);
   glBufferData(GL_ARRAY_BUFFER,sizeof(float)*18,vertices,GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
   
   glGenBuffers(1, &uv_data);
   glBindBuffer(GL_ARRAY_BUFFER, uv_data);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12,uvs, GL_STATIC_DRAW);

   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,0,0);
   
}
void Sprite::End(){
   glDeleteBuffers(1, &vert_data);
   glDeleteBuffers(1, &uv_data);
   glDeleteVertexArrays(1,&vao);
}

void Sprite::draw()
{
  glDisable(GL_CULL_FACE);
  glEnable (GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  
  shader->use();
//trans.elements[13]+=0.0001f;
  transform.update();
  if(texture){texture->bind();}
  else{glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,Shader::twhite);}
  
  glUniform3f(uniTint,color.x,color.y,color.z);
  glUniform1f(shader->uniAlpha,alpha);
  
  glBindVertexArray(vao);
  glUniformMatrix4fv(shader->uniModel, 1, GL_FALSE,transform.model.elements);
  
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);
   
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
}
void Sprite::tick()
{
  draw();
}


Sprite::~Sprite()
{

}

void Sprite::render(float x, float y, float w, float h,vec3 color, float alpha,Texture* texture)
{
    vec3 tcol=sp->color;
    float talpha=sp->alpha;
    Texture* ttex=sp->texture;
    
    sp->alpha=alpha;
    sp->color=color;
    sp->texture=texture;
    sp->resize(x,y,w,h);
    
    sp->tick();
    sp->alpha=talpha;
    sp->texture=ttex;
    sp->color=tcol;
}

