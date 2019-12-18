#include"Editor.h"
#include"EditorUI.h"
#include"manup.h"
#include<GL/glu.h>
void drawIcons();
void VertexSnap();
Skybox* skb;
Manupulator* manupulator;
Button* Editor::icon=0;
EditorUI* Editor::eui;
bool Editor::showui=true;
quat torot;vec3 topos;
#define max(a,b) a<b?b:a
#define norm(a) a<0?0:(a>1?1:a)
Scene* Editor::actScene=0;      // actual edited scene (Neither "common" nor "prefabs")
Editor::Editor():play(0),wireframe(0)
{
  loadPrefs();
  reloadAss();
}
void Editor::reloadAss()
{
  loadAss(Gdir,Rpath);
}

void ewindow_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);

    float rx=width/2.0f;
    float ry=height/2.0f;
    mat4 vv=mat4::Orthographic(-rx,rx,-ry,ry,0.0f,1000.0f);
    Shader* sh=Shader::findShader("render2D");
    sh->use();
    glUniformMatrix4fv(glGetUniformLocation(sh->PID,"ortho"), 1, GL_FALSE, vv.elements);

    Editor::eui->resize();
    Game::fpsmeter->setPos(rx-120.0,ry-22.0);
    Camera::default_camera->proj=mat4::Perspective((60.0f)*M_PI/180.0f, rx/ry, 0.1f, 10000.0f);
}
GLuint fbo;GLuint fbtex;
Sprite* fsp;
void Editor::Init()
{
    topos=Camera::default_camera->pos;
     torot=Camera::default_camera->rot;
     
    if(!Scene::prefabs){Scene::prefabs=new Scene();Scene::prefabs->name="_prefabs_";Scene::scenes.push_back(Scene::prefabs);}
    if(!Scene::common){Scene::common=new Scene();Scene::common->name="_common_";Scene::scenes.push_back(Scene::common);}
    Scene::activeScene->setActive();
    glfwSwapInterval(0);

    grid=new Grid(Shader::findShader("quad"));
    manupulator=new Manupulator;

    skb=new Skybox();
    if(CubeMap::cubemaps.size())skb->cm=CubeMap::cubemaps[0];

    eui=new EditorUI();
    icon=new Button(0,0,0,0);

    glfwSetWindowSizeCallback(window->GL_window, ewindow_size_callback);
    glfwSetWindowTitle(window->GL_window,Gdir.c_str());
    /// fbo experiment

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);


    glGenTextures(1, &fbtex);
    glBindTexture(GL_TEXTURE_2D, fbtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,fbtex, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        printf("frame buffer complete !!!\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     fsp=new Sprite(-360,160,800,-600);
//     fsp->texture=data->textures[1];
//     fsp->texture->texID=fbtex;
}

vec3 pcpt[3];int a=3;


Editor::~Editor()
{
    printf("^^^^^^^^^^^^^^^^^ EDitor destructor \n");
  reloadAss();                 // enable reload if you want scenes to be updated
                           // in res file without restarting editor
  delete eui;
  delete grid;delete icon;delete manupulator;
}
bool isactive(Entity* tent){
    Entity* par=tent;
    while(par){
        if(EMenu::activeEntity==par){return true;}
        par=par->parent;
    }
    return false;
}
void Editor::e_render(Scene* scn){
    Light::setAmbient(Shader::def,vec3(0.7));
    Camera* actcam=Camera::active_camera;
    Camera::active_camera=Camera::default_camera;
    skb->tick();
    //// RENDER SCENE
    Transform tr;
    EMenu* emenu=EMenu::emenu;
    scn->refresh();
    //////////////////
    for(int i=0;i<scn->entities.size();i++){
        Entity* go=scn->entities[i];
        go->update();
        Transform &trans=go->rtrans;
        if(go->enabled){
            if(go->light){
                go->light->position=trans.position;
                go->light->direction=quat::Rotate(trans.rotation,vec3(0.0,0.0,1.0));
            }
            if(go->camera||(go->light&&go->light->type==T_DIRECT)){
                Transform ttra=trans;
                vec3 acolour=go->light?vec3(1.0,1.0,0.5):vec3(1.0,0.5,0.4)-vec3(0.25)*abs(sin(Input::time*10.0));
                ttra.scale=(camera->pos.Distance(trans.position)*0.1f);
                ttra.scale.z*=-1.0;ttra.update();
                mat4 mvp=camera->proview*ttra.model;
                Shader::simple->use();
                glUniform4fv(glGetUniformLocation(Shader::simple->PID,
                            "color"),1,vec4(acolour,1.0).elements);
                glBindVertexArray(manupulator->trans->vao);
                glUniformMatrix4fv(Shader::simple->uniMVP, 1, GL_FALSE, (mvp.elements));
                glEnable(GL_DEPTH_TEST);
                glDrawElements(GL_TRIANGLES,manupulator->trans->es , GL_UNSIGNED_SHORT, 0);
                glDisable(GL_DEPTH_TEST);
           }
        }
    }
    for(int i=0;i<scn->entities.size();i++){
      Entity* go=scn->entities[i];
      Transform &trans=go->rtrans;
      if(go->isEnabled()){
        if(go->sound){go->sound->tick(go->transform.position);}
        if(go->button&&showui){
            go->button->transform.rotation=go->transform.rotation;
            if(go->button->type==2)((Slider*)go->button)->tick();
            else
               go->button->tick();
        }
        
        if(go->rigidBody){
            // set no sleep and zero velocity
            go->rigidBody->setLinearVelocity(btVector3(0,0,0));

            Entity* ent=go;
            if(go->name=="terrain"){
                go->rigidBody->setCollisionFlags(go->rigidBody->getCollisionFlags()|btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
            }
            quat rot=trans.rotation;vec3 pos=trans.position;
            btTransform et(btQuaternion(rot.x,rot.y,rot.z,rot.w), btVector3(pos.x, pos.y, pos.z));
            ent->motionState->setWorldTransform(et*ent->motionState->m_centerOfMassOffset.inverse());
            ent->rigidBody->setMotionState(ent->motionState);
        }
      }
    }
//     if(Camera::active_camera->pos.y>0)
//         grid->draw();
    scn->render();
    if(Scene::common&& scn!=Scene::common && scn!=Scene::prefabs)Scene::common->render();
    if((scn==Scene::common)&&actScene)actScene->render();
//     if(Camera::active_camera->pos.y<=0)
        grid->draw();
  /////////////////////
    VertexSnap();
//     if(emenu->activeEntity)
    manupulator->tick();
    // physics draw
    Entity::dynamicsWorld->debugDrawWorld();
    drawIcons();

  /////////////////////////////////////
    if(counter->deltaTime>=1.0)
        printf("Fps = %f \n",counter->fps);
    Camera::active_camera=actcam;


}
bool phydraw=false;
void Editor::Render()
{
    running=!glfwWindowShouldClose(window->GL_window);

    glClearColor(0.2f,0.5f,0.9f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if(!Scene::play||Camera::active_camera==Camera::default_camera){camera->update(0);}
    play=Scene::play;
//     glEnable(GL_FRAMEBUFFER_SRGB);
    if(Scene::play){
        skb->tick();
        // step physics
        Entity::dynamicsWorld->stepSimulation(Input::deltaTime*Input::timescale,4);
        //render scene
        Scene::activeScene->tick();
//         grid->draw();
        // physics draw
        if(phydraw)Entity::dynamicsWorld->debugDrawWorld();
    }
    else{e_render(Scene::activeScene);}

    eui->update();
    handleInput();

    glfwSwapBuffers(window->GL_window);
    Button::b2=Button::b1;
}

 float camspeed=0.1f;
Camera* plac=0;
void Editor::handleInput()
{
  if(!Scene::play||camera==Camera::active_camera)editor_camera();

  float rx=Shader::sc_res.x/2;
  float ry=Shader::sc_res.y/2;
  char buf[512];

  if(Input::getKeyDown(LCTRL,KEY_I)){wireframe=!wireframe;}
  if(Input::getKeyDown(KEY_F1)||Input::getKeyDown(KEY_F5)||eui->play->justDown){Play();}
  if(input.getKeyDown(KEY_F2)||(Input::getKeyDown(KEY_LCTRL,KEY_R))||eui->reload->justDown)
    {Reload();}
    
  bool bweb=Input::getKeyDown(KEY_F4),blin=Input::getKeyDown(KEY_F3),bwin=Input::getKeyDown(KEY_F9);
  if (bweb||blin||bwin){
    reloadAss();
    std::string plat="lin";
    if(bweb){plat="web";}
    else if(bwin){plat="win";}
    snprintf(buf, sizeof(buf), "cd %s/bin/ && make %s RPATH=./ GPATH=%s",Rpath.c_str(),plat.c_str(),Gdir.c_str());
    system(buf);
    if(bweb){
        snprintf(buf, sizeof(buf), "firefox %s/index.html",Gdir.c_str());
        system(buf);
    }
    else if(blin){
        snprintf(buf, sizeof(buf), "%s/Game",Gdir.c_str());
        system(buf);
    }
    else if(bwin){
        snprintf(buf, sizeof(buf), "wine %s/Game.exe",Gdir.c_str());
        system(buf);
    }
  }
    bool pdwn=eui->prefab->toggle=Scene::activeScene==Scene::prefabs;
    bool cdwn=eui->common->toggle=Scene::activeScene==Scene::common;
    bool ndwn=!pdwn&&!cdwn;
    bool bprefab=eui->prefab->justDown||Input::getKeyDown(KEY_1);
    bool bcommon=eui->common->justDown||Input::getKeyDown(KEY_2);
    if (bprefab ||bcommon ){
        if(ndwn){
            EMenu::pscnname=Scene::activeScene->name;
            actScene=Scene::activeScene;
//             Scene::prefabs->setActive();
        }
        if(bprefab && (cdwn||ndwn))Inspector::scomp->sceneSelect->setActive("_prefabs_");
        else if(bcommon && (pdwn||ndwn)) Inspector::scomp->sceneSelect->setActive("_common_");
        else if(!ndwn){
//             Scene::prefabs->setActive();
            Inspector::scomp->sceneSelect->setActive(EMenu::pscnname);
        }
        
        EMenu::emenu->reset();
        Inspector::refresh();
    }
 
  if(eui->open->justDown||eui->newproj->justDown){
      loadPrefs(false);
      Reload();
      glfwSetWindowTitle(window->GL_window,Gdir.c_str());
      eui->open->justDown=false;eui->newproj->justDown=false;
  }
  if(Input::getKeyDown(LCTRL,KEY_Q)){running=false;}
  if(Input::getKeyDown(KEY_KP_SUBTRACT)){phydraw=!phydraw;}
  if(Input::getKeyDown(LCTRL,KEY_U)){showui=!showui;}
  if(Input::getKeyDown(LCTRL,KEY_S)||eui->fimenu->tbuttons[3]->justDown){
   Save();
 }
    char fbuff[20];
    snprintf(fbuff, sizeof(fbuff), "FPS=%.2f",counter->fps);
    fpsmeter->setText(fbuff);
    fpsmeter->tick();
    counter->update();

}

void Editor::Reload(){
    //stop playing if so
    if(Scene::play){Play();}
    reloadAss();data->path=Gdir+"out.res";
    EMenu::activeEntity=0;
    EMenu::emenu->copied.clear();
    EMenu::emenu->selected.clear();
    data->reload();
    ((EList<Scene>*)eui->scnwindow->menus[0])->refresh();
    delete EMenu::emenu->inspector;
    EMenu::emenu->inspector=new Inspector();
    EMenu::emenu->reset();
    Inspector::refresh();
    eui->resize();
}
float thetay=0,thetax=0;
bool tra=false;float cidst=8;vec3 targp;
void Editor::editor_camera()
{
    mat4 &view=camera->view;
    vec3 &forward=camera->forward,&strafe=camera->strafe,&pos=camera->pos;
    vec3 &tr=Manupulator::scright,&tup=Manupulator::scup;float tsped=0.4;
    Entity * ae=EMenu::activeEntity;
    view=mat4(1.0);
    bool mouseInEditor=Shader::scrMousePos().x>EMenu::emenu->x+EMenu::emenu->width && Shader::scrMousePos().x<Inspector::x;

   float df=0,ds=0;
   float camspeed=24.0f*Input::deltaTime;
   if(!Input::typing){
       //cam directions
        tr= vec3(0,-1,0).Cross(camera->dir).Normalize();
        if (tr==vec3()){tr=quat::Rotate(camera->rot,vec3(1,0,0));}
        tup=camera->dir.Cross(tr).Normalize();
        
            if(Input::getKeyDown(KEY_KP_5)){
                camera->ortho=!camera->ortho;
                if(camera->ortho){
                    float tw=16.0*camera->size,th=9.0*camera->size;
                    camera->proj=mat4::Orthographic(-tw,tw,-th,th,0.0,1000.0);
                }
                else
                    camera->proj=mat4::Perspective((60.0)*M_PI/180.0, 16.0f/9.0, 0.1, 1000.0);
            }
        
            bool cdchange=true;float tcdist=-24;
            if (Input::getKeyDown(KEY_KP_7)){
                torot=quat::FromEulerAngles(vec3(1.57,0,0));
            }
            else if (Input::getKeyDown(KEY_KP_1)){
                torot=quat::FromEulerAngles(vec3(0,0,0));
            }
            else if (Input::getKeyDown(KEY_KP_3)){
                torot=quat::FromEulerAngles(vec3(0,1.57,0));
            }
            else{cdchange=false;}
            if(Input::getMouseButton(2)&&!Input::getKey(KEY_LSHIFT)){
                    
                    vec2 md=Input::getMousedelta();
                    vec3 up(0,1,0);
                    if(ae)
                        targp=ae->rtrans.position;
                    vec3 targ=targp;
                    vec3 dist=(targ-camera->getCamPos());
                    float cfac=1.0;
                    if(dist.Dot(up)>0)cfac=-1;
                    
                    vec3 roll=dist.Cross(up).Normalize();
                    torot=camera->rot*quat(md.y/100.0,roll);
                    torot=torot*quat::FromEulerAngles(vec3(0,md.x/100.0,0));
                    tcdist=-targ.Distance(camera->getCamPos());
                    cdchange=true;
            }
            
            if(cdchange&&ae){
//                 float dist=-(ae->rtrans.position-camera->getCamPos()).Magnitude();
                vec3 tcdir=quat::Rotate(torot.inverse(),-vec3::Zaxis());
                topos=ae->rtrans.position+tcdir*tcdist;
            }
            
        if(Input::getKey(KEY_LSHIFT))tsped*=8.0;
        
        if(camera->ortho&&(!ae||!ae->gridmap))
        {camera->size+=Input::scroll*tsped*80.0;
            camera->size=max(0,camera->size);
            float tw=16.0*camera->size,th=9.0*camera->size;
            camera->proj=mat4::Orthographic(-tw,tw,-th,th,0.0,1000.0);
        }
        else if(mouseInEditor)
        {topos+=camera->dir*Input::scroll*tsped*8.0;}
        
            
        if(mouseInEditor)
        if(Input::getKey(KEY_SPACE)||(Input::getKey(KEY_LSHIFT)&&Input::getMouseButton(2))){
//                 camspeed*=4;
                //if(Input::getMouseButton(2)){
                    tsped/=8;
                    topos+=tr *-Input::getMousedelta().x*tsped;
                    topos+=tup*-Input::getMousedelta().y*tsped;
                //}
        }
        if(Input::getKey(KEY_LCTRL)){camspeed/=4;}

        if(Input::getKey(KEY_W)){df=camspeed;}
        if(Input::getKey(KEY_A)){ds=-camspeed;}
        if(Input::getKey(KEY_S)){df=-camspeed;}
        if(Input::getKey(KEY_D)){ds=camspeed;}

        if(Input::getKey(KEY_Q)){topos.y+=-camspeed;}
        if(Input::getKey(KEY_E)){topos.y+=camspeed;}
   }
  camera->rot=quat::slerp(torot,camera->rot,0.4);
  camera->pos=vec3::lerp(topos,camera->pos,0.4);
  view[12]=-pos.x;
  view[13]=-pos.y;
  view[14]=-pos.z;

  if(Input::getMouseButton(1))
  {
   thetay= Input::getMousedelta().x *0.005;
   thetax= Input::getMousedelta().y *0.005;
   torot= quat::RotationX(thetax)*camera->rot*quat::RotationY(thetay);
  }
  view=mat4::Rotate(camera->rot)*view;
  camera->dir=quat::Rotate(camera->rot.inverse(),-vec3::Zaxis());

   forward= vec3(-view.elements[2],-view.elements[6],-view.elements[10]);
   strafe=  vec3(view.elements[0],view.elements[4],view.elements[8]);

  topos+=forward* (df);
  topos+=strafe* (ds);

}
void Editor::Play()
{
    if(!Scene::play){// start playing!
            tactive=Scene::activeScene;
            if(Scene::activeScene==Scene::common|| Scene::activeScene==Scene::prefabs){
                tactive=actScene;
            }
            tactive->Play();
            Scene::play=true;
//             glfwSetInputMode(window->GL_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else{//stop playing
//         glfwSetInputMode(window->GL_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        Scene::play=false;
        tactive->setActive();
        EMenu::emenu->selected.clear();
        EMenu::emenu->reset();
        Inspector::refresh();
    }
}

void Editor::Save()
{
    if(Scene::play){Play();}
    if (!Input::getKey(KEY_LSHIFT)){
        if(Scene::activeScene!=Scene::common&&Scene::activeScene!=Scene::prefabs)
            Scene::activeScene->write(Gdir+"../scenes/");
        Scene::prefabs->write(Gdir+"../scenes/");
        Scene::common->write(Gdir+"../scenes/");
    }
    else{
        for(int i=0;i<Scene::ascenes.size();i++){
             Scene::ascenes[i]->write(Gdir+"../scenes/");
        }
    }
    // save project preferences
    std::ofstream ofs(Gdir+"../scenes/"+".project",std::ios::binary);

    ofs.write((char*)Camera::default_camera->pos.elements,4*3);
    ofs.write((char*)Camera::default_camera->rot.elements,4*4);

    short ns=Scene::ascenes.size();
    ofs.write((char*)&ns,2);
    for(int i=0;i<ns;i++){
        if(Scene::ascenes[i]){
        short n_len=Scene::ascenes[i]->name.length();
        ofs.write((char*)&n_len,1);
        ofs.write((char*)Scene::ascenes[i]->name.c_str(),n_len);
        }
    }
    ofs.close();
    EMenu::emenu->selected.clear();
    EMenu::activeEntity=0;
    Scene::activeScene->setActive();
    EMenu::emenu->reset();
    EMenu::emenu->inspector->refresh();
}


void Editor::loadPrefs(bool startup)
{
    bool change=false;
    std::ifstream ifs(Rpath+".prefs");
    if(startup &&ifs.good()){
        char name[256];short nlen;
        ifs.read((char*)&nlen,2);nlen-=5;name[nlen]='\0';
        ifs.read((char*)name,nlen);
        Gdir.assign(name);
    }
    else{
        const char* gamedir=tinyfd_selectFolderDialog("select game project","/home");
        if (gamedir){
            Gdir.assign(gamedir);change=true;
        }
    }
    struct stat st;stat(Gdir.c_str(),&st);// If selected directory is invalid
    if(Gdir==Rpath ||(!change && !S_ISDIR(st.st_mode))){Gdir=Rpath+"example/";change=true;}

    mkdir((Gdir+"/out").c_str(),S_IRWXU);
    mkdir((Gdir+"/assets").c_str(),S_IRWXU);
    mkdir((Gdir+"/scenes").c_str(),S_IRWXU);
    if(startup||change){
        Gdir+="/out/";
        if(change)savePrefs();
    }
    //glfwSetWindowTitle(window->GL_window,Gdir.c_str());
}

void Editor::savePrefs()
{
    std::ofstream ofs(Rpath+"/.prefs");
    short nlen=Gdir.length();
    ofs.write((char*)&nlen,2);
    ofs.write((char*)Gdir.c_str(),nlen);

}




