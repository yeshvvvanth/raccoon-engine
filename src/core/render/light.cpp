#include"light.h"
#include"../main/scene.h"
// Shader* Light::shader=0;
GLuint Light::ShadowBuffer=0,Light::depthMap=0,Light::shadowCubeFbo=0,Light::m_shadowMap=0;
short Light::num=0,Light::dirlradius=42;
std::vector<Light*> Light::lights;
#define max(a,b) a<b?b:a
#define norm(a) a<0?0:(a>1?1:a)
void Light::setAmbient(Shader *shader,vec3 col)
{
  shader->use();
  glUniform3fv(shader->uniAmbient,1,col.elements);
}

void motivate(vec3& col){}

Light::Light(char type):type(type)
{
  radius=20.0f;
  colour=vec3(0.9);
  direction=vec3(1.0,0.0,0.0);
  angle=0.0;
  intensity=1.0;
  lights.push_back(this);
  shadow=false;
}


void Light::update(Shader *shader,char type)
{
  int n=num;if(type==0)n=4;
  if(num<NUM_PLIGHTS){
    
    colour.x=max(colour.x,0);
    colour.y=max(colour.y,0);
    colour.z=max(colour.z,0);
    
    shader->use();
    glUniform3fv(shader->uniLightCol[n],1,colour.elements);
    glUniform1i(shader->uniLightType[n],type);
    
    if(type==T_POINT || type==T_SPOTLIGHT){
        glUniform3fv(shader->uniLightPos[n],1,position.elements);
        glUniform1f(shader->uniLightRadius[n],max(radius,0));
    }
    vec3 dir=direction.Normalize();
    glUniform3fv(shader->uniLightDir[n],1,dir.elements);
    if(type)Light::num++;
  }
}
void Light::reset(Shader *shader)
{
    shader->use();
    Light::num=0;
    vec3 tp,tc;
    for(int n=0;n<NUM_PLIGHTS;n++){
        glUniform3fv(shader->uniLightPos[n],1,tp.elements);
        glUniform3fv(shader->uniLightCol[n],1,tc.elements);
        glUniform1i(shader->uniLightType[n],0);
        glUniform1f(shader->uniLightRadius[n],0.0);
    }
}

Light::~Light()
{
    
}
/////// shadow implementation //////////

void Light::doShadow(vec3 pos,float *bounds){
    position=pos;
    Shader*sh=Shader::findShader("shadow");
    Shader*psh=Shader::def;

//     mat4 view=mat4::LookAt(position,vec3(0.0,0.0,0.0),vec3( 0.0f, 1.0f,  0.0f)); 
    mat4 view=mat4(1.0);
    quat rot(direction,vec3::Zaxis());
    view[12]=-position.x;
    view[13]=-position.y;
    view[14]=-position.z;
    
    view=mat4::Rotate(rot)*view;
    
    mat4 proj=mat4::Orthographic(-bounds[0],bounds[1],-bounds[2],bounds[3],-bounds[4],bounds[5]);
    mat4 pv=proj*view;
    sh->use();
//     glEnable(GL_CULL_FACE);
//     glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowBuffer); 
    vec2 sc_res=Shader::getResolution();
    glViewport(0,0,Shader::sh_res.x,Shader::sh_res.y);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    for(int i=0;i<Scene::activeScene->entities.size();i++){
      Entity* go=Scene::activeScene->entities[i];
        if(go->enabled&&go->mesh&&go->mesh->type==1 && go->mesh->flags&DOSHADOW){
            Mesh* mesh=(Mesh*)go->mesh;
            RawMesh* me=mesh->rawmesh;
            mat4 mvp=pv*go->rtrans.model;
            
            glBindVertexArray(me->vao);
            if(me->type&MESH_ANIM){
                {
                  me->arm->active_anim=mesh->anum;
                  me->arm->animate(0,mesh->f,mesh->ff,false);
                }
                  glUniform1f(sh->uniRigged,1.0f);
                  glUniformMatrix4fv(sh->uniAnim,maxbones,GL_FALSE,me->arm->matrices);
            }
            else{
                 glUniform1f(sh->uniRigged,0.0f);
            }
            glUniformMatrix4fv(sh->uniMVP, 1, GL_FALSE, (mvp.elements));
            glDrawElements(GL_TRIANGLES,me->es , GL_UNSIGNED_SHORT, 0);
        }
    }
//     glCullFace(GL_BACK);
//     glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glViewport(0,0,sc_res.x,sc_res.y);
    psh->use();
//     glUniform3fv(glGetUniformLocation(psh->PID,"lightDir"),1,(direction).elements);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,depthMap);
    glActiveTexture(GL_TEXTURE0);
    glUniformMatrix4fv(psh->uniLightMat, 1, GL_FALSE, (pv.elements));
    Shader* tsh=Shader::findShader("terrain");
    tsh->use();
    glUniformMatrix4fv(tsh->uniLightMat, 1, GL_FALSE, (pv.elements));
}
/*
void Light::doCubeShadow(){

    Shader*sh=Shader::findShader("shadow");
    Shader*psh=Shader::def;

//     mat4 view=mat4::LookAt(position,vec3(0.0,0.0,0.0),vec3( 0.0f, 1.0f,  0.0f)); 
    mat4 view=mat4(1.0);
    view[12]=-position.x;
    view[13]=-position.y;
    view[14]=-position.z;
    
    mat4 proj=mat4::Perspective(PI/2.0,1.0,0.4,10.0);
    glBindFramebuffer(GL_FRAMEBUFFER, Light::shadowCubeFbo);
    sh->use();
//     glEnable(GL_CULL_FACE);
//     glCullFace(GL_FRONT);

    vec2 sc_res=Shader::getResolution();
    glViewport(0,0,512.0,512.0);
    
    vec3 dirs[6];float nt=PI/2.0;
    dirs[0]=vec3(0,nt,nt*2);dirs[3]=vec3(-nt,nt*2.0,nt*2.0);
    dirs[1]=vec3(0,-nt,nt*2);dirs[4]=vec3(0,nt*2,nt*2);
    dirs[2]=vec3(nt,nt*2.0,nt*2.0) ;dirs[5]=vec3(0,0,nt*2);
    
    glEnable(GL_DEPTH_TEST);
    
    for(int u=0;u<6;u++){
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X+u, Light::m_shadowMap, 0);
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        
        mat4 tview=mat4::Rotate(quat::FromEulerAngles(dirs[u]))*view;
        mat4 pv=proj*tview;
        for(int i=0;i<Scene::activeScene->entities.size();i++){
            Entity* go=Scene::activeScene->entities[i];
            if(go->enabled&&go->mesh&&go->mesh->type==1){
                Mesh* mesh=(Mesh*)go->mesh;
                RawMesh* me=mesh->rawmesh;
                mat4 mvp=pv*go->rtrans.model;
                glBindVertexArray(me->vao);
                
                if(me->type&MESH_ANIM){
                    {
                    me->arm->active_anim=mesh->anum;
                    me->arm->animate(0,mesh->f,mesh->ff,false);
                    }
                    glUniform1f(sh->uniRigged,1.0f);
                    glUniformMatrix4fv(sh->uniAnim,maxbones,GL_FALSE,me->arm->matrices);
                }
                else{
                    glUniform1f(sh->uniRigged,0.0f);
                }
                
                glUniformMatrix4fv(sh->uniMVP, 1, GL_FALSE, (mvp.elements));
                glDrawElements(GL_TRIANGLES,me->es , GL_UNSIGNED_SHORT, 0);
            }
        }
        
    }
//     glCullFace(GL_BACK);
//     glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glViewport(0,0,sc_res.x,sc_res.y);

    psh->use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP,m_shadowMap);
    glActiveTexture(GL_TEXTURE0);
}
    */

