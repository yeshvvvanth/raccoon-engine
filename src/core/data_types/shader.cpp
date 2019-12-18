#include"rawmesh.h"
#include "../main/input.h"
#include"../render/camera.h"
#include"../render/sprite.h"
#include<string.h>
vec2 Shader::sc_res,Shader::sh_res;
GLuint Shader::twhite=0,Shader::lvao=0;
std::vector<Shader*> Shader::shaders,Shader::eshaders;
Shader* Shader::def=0, *Shader::render2D=0, *Shader::simple=0, *Shader::fontShader=0,*Shader::pervert=0,*Shader::terrain=0;
void Shader::Init()
{
   glGenTextures(1,&twhite);
   glBindTexture(GL_TEXTURE_2D,twhite);
   unsigned char pix[3]={255,255,255};
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &pix);
   
   float verts[]={0.0,0.0,0.0,0.0,0.0,1.0};
   GLuint vert_data;
  
   glGenVertexArrays(1, &lvao);
   glBindVertexArray(lvao);
   Shader *shader=findShader("simple");
   shader->use();
  
   glGenBuffers(1,&vert_data);
   glBindBuffer(GL_ARRAY_BUFFER,vert_data);
   glBufferData(GL_ARRAY_BUFFER,sizeof(float)*6,verts,GL_STATIC_DRAW);
   
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
}
vec2 Shader::scrMousePos(bool scale)
{
  vec2 sr=getResolution();
  vec2 mp=Input::mousepos;

  mp=vec2(mp.x-sr.x/2.0,sr.y/2.0-mp.y);
  
  vec2 stres(960.0,540.0);
  /*
  if(scale){
    mp=vec2(mp.x*stres.x/sr.x,mp.y*stres.y/sr.y);
  }
  */
  return mp;
}

Shader::Shader(char* sname,const GLchar* vertexSource,const GLchar* fragmentSource)
{   ///vert
    //vert=vertexSource;frag=fragmentSource;
    name=sname;
    vert=vertexSource;
    frag=fragmentSource;
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,(const GLchar**)&vertexSource,NULL);
    glCompileShader(vertexShader);
	//check
    GLint status=GL_FALSE;
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&status);
    if(status==GL_FALSE){printf("%s :Vertex shader says screw you! \n",sname);}
    else printf("%s :Vertex shader is Fine :) \n",sname);
    
    GLint logLength=0;
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
    char *vlogbuffer=new char[logLength+1];vlogbuffer[logLength]='\0';
    glGetShaderInfoLog(vertexShader, logLength, NULL, vlogbuffer);
    printf(" \n %s  \n ",vlogbuffer);
    delete[] vlogbuffer;
    
    ///frag
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader,1,(const GLchar**)&fragmentSource,NULL);
    glCompileShader(fragShader);
	//check
    status=GL_FALSE;
    
    glGetShaderiv(fragShader,GL_COMPILE_STATUS,&status);
    if(status==GL_FALSE){printf("%s : Fragment shader says screw you! \n",sname);}
    else printf("%s :Fragment shader is Fine :) \n",sname);
    
    
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    char *flogbuffer=new char[logLength+1];flogbuffer[logLength]='\0';
    glGetShaderInfoLog(fragShader, logLength, NULL, flogbuffer);
    printf(" \n %s  \n ",flogbuffer);
    delete[] flogbuffer;
    
    ///combine vet & frag
    PID=glCreateProgram();
    glAttachShader(PID,vertexShader);
    glAttachShader(PID,fragShader);
    
    glBindAttribLocation(PID,0,"position");
    glBindAttribLocation(PID,1,"boneIDs");
    glBindAttribLocation(PID,2,"weights");
    glBindAttribLocation(PID,3,"texcoord");
    glBindAttribLocation(PID,4,"Color");
    glBindAttribLocation(PID,5,"normal");
    glBindAttribLocation(PID,6,"tangent");
    glBindAttribLocation(PID,7,"bitangent");
    glBindAttribLocation(PID,8,"tfactor");
    
    glLinkProgram(PID);
    glUseProgram(PID);
    // matrix,vector and value uniforms 
    uniModel = glGetUniformLocation(PID, "model");
    uniMVP   = glGetUniformLocation(PID, "PVM");
    uniAlpha = glGetUniformLocation(PID, "alpha");
    
    uniSmooth  = glGetUniformLocation(PID,"smoothness");
    uniTile    = glGetUniformLocation(PID,"tile");
    uniTileOff = glGetUniformLocation(PID,"tileoff");
    uniLightMat= glGetUniformLocation(PID,"lightMat");
    
    uniRigged   = glGetUniformLocation(PID,"rigged");
    uniTextured = glGetUniformLocation(PID,"istex");
    uniAnim     = glGetUniformLocation(PID,"anim");
    
    char buff[50];
    
    uniAmbient=glGetUniformLocation(PID, "ambient");
    uniCam=    glGetUniformLocation(PID, "viewPos");
    
    for(int i=0;i<LightsPerFrag;i++){
      sprintf(buff,"lights[%d].pos",i);
      uniLightPos[i]=glGetUniformLocation(PID, buff);
    }
    for(int i=0;i<LightsPerFrag;i++){
      sprintf(buff,"lights[%d].col",i);
      uniLightCol[i]=glGetUniformLocation(PID, buff);
    }
       
    for(int i=0;i<LightsPerFrag;i++){
      sprintf(buff,"lights[%d].dir",i); 
      uniLightDir[i]=glGetUniformLocation(PID, buff);
    }
    for(int i=0;i<LightsPerFrag;i++){
      sprintf(buff,"lights[%d].type",i);
      uniLightType[i]=glGetUniformLocation(PID, buff);
    }
    for(int i=0;i<LightsPerFrag;i++){
      sprintf(buff,"lights[%d].radius",i);
      uniLightRadius[i]=glGetUniformLocation(PID, buff);
    }
        
    if(uniAlpha==GL_INVALID_VALUE){printf("NO ALPHA!!!!\n");}
    
   
    if(uniAlpha!=GL_INVALID_VALUE)glUniform1f(uniAlpha,1.0);
    // texture unit uniforms 

    GLuint texids[7];
    texids[0]= glGetUniformLocation(PID, "tex");
    texids[1]= glGetUniformLocation(PID, "shadowMap");
    texids[2]= glGetUniformLocation(PID, "skybox");
    texids[3]= glGetUniformLocation(PID, "shadowCubeMap");
    texids[4]= glGetUniformLocation(PID, "envMap");
    texids[5]= glGetUniformLocation(PID, "norMap");
    texids[6]= glGetUniformLocation(PID, "specMap");
    for(int i=0;i<7;i++){
        if(texids[i]!=GL_INVALID_VALUE)glUniform1i(texids[i],i);
    }

    glDeleteShader(fragShader);
    glDeleteShader(vertexShader);
    if(sc_res.x==0){sc_res=getResolution();}
}

Shader::~Shader()
{
  delete[] vert;delete[] frag;
  delete[] name;
  glDeleteProgram(PID);
}

void Shader::use(){glUseProgram(PID);}

vec2 Shader::getResolution()
{
  GLint m_viewport[4];
  glGetIntegerv( GL_VIEWPORT, m_viewport );
  sc_res=vec2(m_viewport[2],m_viewport[3]);
  return sc_res;
}

float Shader::getAspectRatio()
{
  GLint m_viewport[4];
  glGetIntegerv( GL_VIEWPORT, m_viewport );
  
  return ((float)m_viewport[2]/(float)m_viewport[3]);
}



Shader* Shader::findShader(const char* sha_name)
{
  for(int i=0;i<shaders.size();i++){
    if(strcmp(sha_name,shaders[i]->name)==0){return shaders[i];}
  }
  for(int i=0;i<eshaders.size();i++){
    if(strcmp(sha_name,eshaders[i]->name)==0){return eshaders[i];}
  }
  return 0;
}

void Shader::drawLine(vec3& sp, vec3& ep, vec3 colour)
{
    Shader *shader=Shader::simple;
    shader->use();
    glBindVertexArray(lvao);
    glEnable(GL_DEPTH_TEST);

    vec3 d1=vec3::Zaxis(),d2=(ep-sp).Normalize();
    
    Transform tr;
    float s=(ep-sp).Magnitude();
    tr.scale=vec3(1.0,1.0,s);
    tr.position=sp;
    tr.rotation=quat(d1,d2);

    tr.update();
    vec4 color(colour,1.0);
    glUniform4fv(glGetUniformLocation(shader->PID, "color"),1,color.elements); 
    mat4 mvp=Camera::active_camera->proview*tr.model;
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
//     glUniformMatrix4fv(shader->uniModel, 1, GL_FALSE, (tr.model.elements));        
    
    glDrawArrays(GL_LINES,0,2);
    glDisable(GL_DEPTH_TEST);
    
}

void Shader::renderLine(vec3& sp, vec3& ep,float th1,float th2, vec4 colour,vec4 colour2){
        Shader *shader=Shader::simple;
        printf("rendering line!!! \n");
//         RawMesh* rm=RawMesh::findRawMesh("primitives#quad",1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        shader->use();
        glBindVertexArray(Sprite::vao);
        glEnable(GL_CULL_FACE);
        vec3 cp=Camera::active_camera->pos;
        vec3 d1=vec3::Yaxis(),d2=(ep-sp).Normalize();
        vec3 cdir=((cp-sp).Cross(ep-cp)).Normalize();
        cdir=cdir.Cross(sp-ep).Normalize();
        
        Transform tr;
        tr.scale=vec3(th1,(ep-sp).Magnitude(),th1);
        tr.position=sp-d2.Cross(cdir)*th1*0.5; //offset adjustment
        tr.rotation=quat(d1,d2);
        
        vec3 v=quat::Rotate(tr.rotation,vec3::Zaxis());
        tr.rotation=quat(v.Normalize(),cdir)*tr.rotation;

        tr.update();
        
        mat4 mvp=Camera::active_camera->proview*tr.model;
        glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
        glUniform4fv(glGetUniformLocation(shader->PID, "color"),1,colour.elements); 
        glUniform4fv(glGetUniformLocation(shader->PID, "color2"),1,colour2.elements); 
        glUniform1f(glGetUniformLocation(shader->PID, "grad"),1);
        
        glDrawArrays(GL_TRIANGLES,0,6);
//         glDrawElements(GL_TRIANGLES,rm->es , GL_UNSIGNED_SHORT, 0);
        glUniform1f(glGetUniformLocation(shader->PID, "grad"),0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
}

void Shader::renderPrim(short type, Transform& trans, vec4 colour)
{
    glEnable(GL_BLEND);
    Shader* shader=Shader::simple;
    RawMesh* rm=RawMesh::findRawMesh("primitives#cube",1);

    if(type==1)
        rm=RawMesh::findRawMesh("primitives#sphere",1);
    else if(type==2)
        rm=RawMesh::findRawMesh("primitives#cylinder",1);
    
    shader->use();
    glUniform4fv(glGetUniformLocation(shader->PID, "color"),1,colour.elements); 
    mat4 mvp=Camera::active_camera->proview*trans.model;
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
    rm->render(trans);
    glDisable(GL_BLEND);
}

