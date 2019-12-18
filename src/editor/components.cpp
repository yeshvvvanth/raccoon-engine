#include"entitymenu.h"

LightComp* AddComp::lc=0;
MeshComp* AddComp::mc=0;
SoundComp* AddComp::ac=0;
camComp* AddComp::cc=0;
UIComp* AddComp::uc=0;
PhysicsComp* AddComp::pc=0;
ScriptComp* AddComp::sc=0;
///       TRANSFORM COMPONENT     //////////////////////////
Transpector::Transpector()
{
  type=(1);
  
  props[0]=new Prop_Vector("Position",vec3(0.0));
  props[1]=new Prop_Vector("Rotation",vec3(0.0));
  props[2]=new Prop_Vector("Scale",vec3(1.0));

  for(int i=0;i<3;i++){
      TextButton* tb;
      tb=((FtextField*)props[i]->buttons[0])->tb;
      tb->bgColor=(vec3(0.8,0.34,0.3))*0.9;tb->activeColor=tb->bgColor*1.8;
      tb=((FtextField*)props[i]->buttons[1])->tb;
      tb->bgColor=(vec3(0.5,0.7,0.4)) *0.9;tb->activeColor=tb->bgColor*1.8;
      tb=((FtextField*)props[i]->buttons[2])->tb;
      tb->bgColor=(vec3(0.4,0.6,0.8)) *0.9;tb->activeColor=tb->bgColor*1.8;
  }

  if(EMenu::activeEntity)get();
}
void Transpector::sort(){
  H=-5;
  y=Inspector::y+Inspector::H;
  for(int i=0;i<3;i++){
    props[i]->sort(H);
  }
  Inspector::H+=H;
}
Transpector::~Transpector()
{
  for(int i=0;i<3;i++){
    delete  props[i];
  }
}

void Transpector::set()
{
  Entity* go=EMenu::activeEntity;
  
  go->transform.position=props[0]->toVec();
  go->transform.rotation=quat::FromEulerAngles(props[1]->toVec()*float(PI/180.0f));
  if(!EMenu::activeEntity->source.length())
    go->transform.scale=props[2]->toVec();
  else if(go->transform.scale!=props[2]->toVec())
  {get();}

  if(go->rigidBody && go->parent && go->parent->rigidBody && 
      go->parent->rigidBody->getCollisionShape()->getShapeType()==COMPOUND_SHAPE_PROXYTYPE)
  {
      
  }
}
void Transpector::get(){
   Entity* go=EMenu::activeEntity;
   if(go){
    vec3& p=go->transform.position;
    vec3  r=go->transform.rotation.ToEulerAngles()*float(180.0f/PI);
    vec3& s=go->transform.scale;

    
   props[0]->fromVec(p);
   props[1]->fromVec(r);
   props[2]->fromVec(s);
   empty=false;
   }
   else {empty=true;}
}
void Transpector::tick()
{
   Entity* go=EMenu::activeEntity;
  if(go){
    if((EMenu::emenu->dragged)){get();}
    vec3& p=go->transform.position;
    vec3  r=go->transform.rotation.ToEulerAngles();
    vec3& s=go->transform.scale;
    if((props[0]->toVec()!=p||props[1]->toVec()!=r||props[2]->toVec()!=s ) && !Scene::activeScene->play){
       set();
    }
//     if(Scene::activeScene->play){get();}
//     if(Scene::activeScene->play){Transform tt=go->transform;go->transform=go->rtrans;get();go->transform=tt;}
  }
  for(int i=0;i<3;i++){
    props[i]->tick();
  }
}
void setMeshComp(char type,std::string name=""){
        Entity* ae=EMenu::activeEntity;
    if(type==1){
        MeshComp *mc=AddComp::mc;
        mc->empty=false;
        mc->meshSelect->selMenu->setActive(name);

        if(ae->mesh&&ae->mesh->type==1)
            ((Mesh*)ae->mesh)->rawmesh=RawMesh::findRawMesh(name,0);
        else
                ae->mesh=new Mesh(RawMesh::findRawMesh(name,0));
        //    printf("ins H= %d \n\n",Inspector::H);
    }
    else if(type==2){
        if(ae->mesh)delete (ae->mesh);
        ae->mesh=new Quad();
    }
    else if(type==3){
        if(ae->mesh)delete (ae->mesh);
        ae->mesh=new Terrain();
    }
   Inspector::refresh();
}
void setScriptComp(std::string name){
    ScriptComp *sc=0;
    Script* scr=0;
    Entity* ae=EMenu::activeEntity;
    if(ae&&!ae->script){
        for(int i=0;i<Script::scripts.size();i++){
            Script* s=Script::scripts[i];
            if(s->name==name){scr=s;break;}
        }
        ae->script=scr->clone();ae->script->entity=ae;
        AddComp::sc->get();
        Inspector::sort();
    }
}
void setLightComp(std::string name){
  if(EMenu::activeEntity->light){delete EMenu::activeEntity->light;}
  if(name=="Point Light"){AddComp::lc->title->setText(name);EMenu::activeEntity->light=new Light(T_POINT);}
  else if(name=="Directional Light"){AddComp::lc->title->setText(name);EMenu::activeEntity->light=new Light(T_DIRECT);}
  else if(name=="Spot Light"){AddComp::lc->title->setText(name);EMenu::activeEntity->light=new Light(T_SPOTLIGHT);}

  AddComp::lc->get();
  Inspector::sort();
}
void setSoundComp(std::string name){
    EMenu::activeEntity->sound=new Sound(Audio::findAudio(name));
    AddComp::ac->get();
     Inspector::sort();
}
void setUIComp(std::string tname){
    Entity *e=EMenu::activeEntity;
    bool b=true;
    if(tname=="Button"){
        e->button=new Button(0,0,70.0,30.0);
    }
    else if(tname=="Text Button"){
        e->button=new TextButton(0,0,"text");
    }
    else if(tname=="Label"){
        e->button=new TextButton(0,0,"text",0,true);
    }
    else if(tname=="Sprite"){
        e->button=new Button(0,0,70.0,30.0);e->button->type=6;
    }
    else if(tname=="Slider"){
        e->button=new Slider(0,0,120.0,10.0);
    }
    else{b=false;}
    
   if(b){   
       AddComp::uc->title->setText(tname);
       AddComp::uc->empty=false;
       Inspector::refresh();
   }
}

void setPhysicsComp(std::string tname){
    
    bool b=false;
    Entity* ent=EMenu::activeEntity;
    quat rot=ent->transform.rotation;vec3 pos=ent->transform.position;
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    if(ent && ent->rigidBody==0){
        b=true;
        if(tname=="Box Collider"){
            btVector3 halfdim(2,2,2);
            ent->collShape=new btBoxShape(halfdim);
        }
        else if(tname=="Cylinder Collider"){
            btVector3 halfdim(2,4,2);
            ent->collShape=new btCylinderShape(halfdim);
        }
        else if(tname=="Sphere Collider"){
             ent->collShape=new btSphereShape(2.5f);
        }
        else if(tname=="Capsule Collider"){
            ent->collShape=new btCapsuleShape(1.5,5.0);
        }
        else if(tname=="Plane Collider"){
            ent->collShape=new btStaticPlaneShape(btVector3(0, 1, 0),0);
        }
        else if(tname=="Empty Collider"){
            ent->collShape=new btEmptyShape();
        }
        else if(tname=="Convex Hull"&&ent->mesh&&ent->mesh->type==1){
            ent->collShape=((Mesh*)ent->mesh)->getCollider();
            ent->collShape->setLocalScaling(toBVec3(ent->rtrans.scale));
        }
        else if(tname=="TriangleMesh"&&ent->mesh&&ent->mesh->type==1){
            ent->collShape=((Mesh*)ent->mesh)->getCollider(1);
            ent->collShape->setLocalScaling(toBVec3(ent->rtrans.scale));
        }
        else if(tname=="Terrain Collider"&&ent->mesh&&ent->mesh->type==3){
            mass=0.0;
            ent->collShape=((Terrain*)ent->mesh)->getCollider();
            
        }

        else{b=false;}
    }
    if(b){
       ent->collShape->calculateLocalInertia(mass, fallInertia);
       btTransform et(btQuaternion(rot.x,rot.y,rot.y,rot.w), btVector3(pos.x, pos.y, pos.z));
        
       ent->motionState=new MotionState(et);
       ent->rigidBody=new btRigidBody(mass,ent->motionState,ent->collShape,fallInertia);
       ent->dynamicsWorld->addRigidBody(ent->rigidBody);
        
       if(tname=="Terrain Collider"){
        ent->rigidBody->setCollisionFlags(ent->rigidBody->getCollisionFlags()|btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
       }
       AddComp::pc->title->setText(tname);
       AddComp::pc->empty=false;
       Inspector::refresh();
    }
    
}


///             Entity  comp              //////////////////////////
EntComp::EntComp()
{
  type=(2);
  entname=new Prop_String("Name","pika");
  enable=new Button(0,0,20,20);
  enable->isToggle=true;
  enable->aColor=vec3(0.6,0.8,0.4)*1.2;
  enable->uColor=vec3(0.6);
   if(EMenu::activeEntity)get();
}

void EntComp::get()
{
  empty=EMenu::activeEntity==0;
  if(!empty){
      entname->field->label->setText(EMenu::activeEntity->name);
      enable->toggle=EMenu::activeEntity->enabled;
  }
}
void EntComp::sort()
{
  y=Inspector::y+Inspector::H;
  H=-40;
  Inspector::x+=50;
  entname->sort(H);
  Inspector::x-=50;
  enable->transform.position.x=entname->label->x-40;
  enable->transform.position.y=entname->label->y;
  Inspector::H+=H;
}
void EntComp::set()
{
    EMenu::activeEntity->enabled=enable->toggle;
    if(EMenu::activeEntity->name != entname->field->label->text){
        EMenu::activeEntity->name = Scene::activeScene->newname(entname->field->label->text);
        if(entname->field->label->text!=EMenu::activeEntity->name)
            entname->field->label->setText(EMenu::activeEntity->name);
    }
}
void EntComp::tick()
{
  entname->tick();
  vec3& p=enable->transform.position,&s=enable->transform.scale;
  Sprite::render(p.x-3.0,p.y-3.0,s.x*1.3,s.y*1.3,vec3(0.4));
  enable->tick();
   if(EMenu::activeEntity  && !Scene::activeScene->play){
        if(!EMenu::emenu->dragged)set();
        else get();    
   }
}
EntComp::~EntComp()
{
delete entname;delete enable;
}
int mco=0;
///       MESH COMPONENT ///////////////
MeshComp::MeshComp()
{
  type=(3);mco++;
  label=new Label("Mesh Render",0,0,vec3(1.0f));
  meshSelect=new Prop_Select<RawMesh>("mesh",RawMesh::meshes,true);
  textureSelect=new Prop_Select<Texture>("texture",Texture::textures,false);
  std::vector<Animation*> vec;
  animSelect=new Prop_Select<Animation>("animation",vec,true);
  tile=new Prop_Vec2("tile",vec2(1,1));
  tileoff=new Prop_Vec2("tile offset",vec2(0,0));
  smoothness=new Prop_Float("Smoothness",0);
  tselects[0]=new Prop_Select<Texture>("texture2",Texture::textures,false);
  tselects[1]=new Prop_Select<Texture>("texture3",Texture::textures,false);
  tselects[2]=new Prop_Select<Texture>("texture4",Texture::textures,false);
  tsize=new Prop_Vec2("terrain size",vec2(128,128));
  tsize->buttons[0]->clamp=4;
  tsize->buttons[1]->clamp=4;
  tscale=new Prop_Float("scale",0);
  sframe=new Prop_Float("start frame",0);
  eframe=new Prop_Float("end frame",0);
  aspeed=new Prop_Float("anim speed",0);
  sframe->ftext->clamp=eframe->ftext->clamp=4;
  smoothness->ftext->clamp=1;
  transparent=new Prop_Bool("transparent",0);
  doshadow=new Prop_Bool("do shadow",0);\
  unshaded=new Prop_Bool("unshaded",0);
  depthtest=new Prop_Bool("depth test",0);
  face= new Prop_Bool("face camera",0);
  tile->buttons[0]->clamp=1;
  tile->buttons[1]->clamp=1;
  if(EMenu::activeEntity)get();
}

void MeshComp::tick()
{
  Entity* ae=EMenu::activeEntity;
  label->tick();
  tile->tick();
  tileoff->tick();
  if(ae&&ae->mesh){
    if (expand){
        if(ae->mesh->type==1)
            transparent->tick();
        depthtest->tick();
        doshadow->tick();
        unshaded->tick();
    }
    if(ae->mesh->type==1){
        smoothness->tick();
    }
    label-> color=vec3(0.99,0.8,0.6);
    if(ae->mesh->type==2){aspeed->tick();eframe->tick();sframe->tick();face->tick();}
    else if(ae->mesh->type==3){tsize->tick();tscale->tick();for(int i=0;i<3;i++)tselects[2-i]->tick();}
    textureSelect->tick();
    if(ae->mesh->type==1&&((Mesh*)ae->mesh)->rawmesh->type&MESH_ANIM){
        animSelect->tick();
    }
    if(ae->mesh&&ae->mesh->type==1)meshSelect->tick();
  }
  if(ae&& !Scene::activeScene->play)set();
}

void MeshComp::get()
{
  Entity* ae=EMenu::activeEntity;
  if(ae==0 || ae->mesh==0){
    empty=true;    
  }
  else {
    empty=false;
    transparent->button->toggle=ae->mesh->flags&TRANSPARENT;
    depthtest->button->toggle=ae->mesh->flags&DODEPTH;
    doshadow->button->toggle=ae->mesh->flags&DOSHADOW;
    unshaded->button->toggle=ae->mesh->flags&UNSHADED;
    face->button->toggle=ae->mesh->flags&FACE;
    if(ae->mesh->type==1){ // Mesh
        Mesh* mesh=(Mesh*)ae->mesh;
        meshSelect->selMenu->setActive(mesh->rawmesh->name);
        if(ae->mesh->texture)
            textureSelect->selMenu->setActive(ae->mesh->texture->name);
        
        if(mesh->rawmesh->type&MESH_ANIM){
            delete animSelect;
            animSelect=new  Prop_Select<Animation>("animation",mesh->rawmesh->arm->animations);
            animSelect->selMenu->setActive(mesh->rawmesh->arm->animations[mesh->anum]->name);
        }
        smoothness->ftext->setValue(mesh->smoothness);
        label->setText("Mesh Render");
    }
    else if(ae->mesh->type==2){// Quad
        label->setText("Qaud Render");
        if(ae->mesh->texture)
            textureSelect->selMenu->setActive(ae->mesh->texture->name);
    }
    else if(ae->mesh->type==3){// terrain
        label->setText("Terrain Render");
        if(ae->mesh->texture)
            textureSelect->selMenu->setActive(ae->mesh->texture->name);
        for(int i=0;i<3;i++){
          if(((Terrain*)ae->mesh)->textures[i])  
          tselects[i]->selMenu->setActive(((Terrain*)ae->mesh)->textures[i]->name);
          else
              tselects[i]->selMenu->activeObj=0;
        }
        tsize->fromVec(vec2(((Terrain*)ae->mesh)->sizex,((Terrain*)ae->mesh)->sizey));
        tscale->ftext->setValue(((Terrain*)ae->mesh)->scale);
        
        ptsize=tsize->toVec();ptscale=tscale->ftext->value;
    }
    tile->fromVec(ae->mesh->tile);
    tileoff->fromVec(ae->mesh->tileoff);
  }
}

void MeshComp::set()
{
    Entity* ae=EMenu::activeEntity;
  if(ae&&ae->mesh){
    if(transparent->button->toggle){ae->mesh->flags|=TRANSPARENT;}
    else {ae->mesh->flags&=~TRANSPARENT;}
    if(doshadow->button->toggle){ae->mesh->flags|=DOSHADOW;}
    else {ae->mesh->flags&=~DOSHADOW;}
    if(depthtest->button->toggle){ae->mesh->flags|=DODEPTH;}
    else {ae->mesh->flags&=~DODEPTH;}
    if(unshaded->button->toggle){ae->mesh->flags|=UNSHADED;}
    else {ae->mesh->flags&=~UNSHADED;}
    if(face->button->toggle){ae->mesh->flags|=FACE;}
    else {ae->mesh->flags&=~FACE;}
  }
  if (ae->mesh&& ae->mesh->type==1){// Render Mesh
      Mesh* mesh=(Mesh*)ae->mesh;
    if(mesh->rawmesh!=meshSelect->selMenu->activeObj){
        mesh->rawmesh=meshSelect->selMenu->activeObj;
        Inspector::refresh();
    }
    if(ae->mesh->texture!=textureSelect->selMenu->activeObj){
        ae->mesh->texture=textureSelect->selMenu->activeObj;
    }
    if(mesh->rawmesh->type&MESH_ANIM){
        if(animSelect->selMenu->activeObj->name!=mesh->rawmesh->arm->animations[mesh->anum]->name)
            mesh->setAnimation(-1,animSelect->selMenu->activeObj->name);
    }
    mesh->smoothness=smoothness->ftext->value;
  }
  else if(ae->mesh&& ae->mesh->type==2){//  Quad
      if(ae->mesh->texture!=textureSelect->selMenu->activeObj){
        ae->mesh->texture=textureSelect->selMenu->activeObj;
      }
  }
  else if(ae->mesh&& ae->mesh->type==3){// Terrain
      if(ae->mesh->texture!=textureSelect->selMenu->activeObj){
        ae->mesh->texture=textureSelect->selMenu->activeObj;
      }
      for(int i=0;i<3;i++)
          ((Terrain*)ae->mesh)->textures[i]=tselects[i]->selMenu->activeObj;
          
      vec2 ctsize=tsize->toVec();ctsize=vec2(int(ctsize.x),int(ctsize.y));
      if(ptsize!=ctsize){//resize terrain
          printf("&&&&&&&&&&&&&&&fadkljsjasdfkllll\n");
                
            delete ae->mesh;ptsize=ctsize;
            ae->mesh=new Terrain(int(ptsize.x),int(ptsize.y));
      }  
      ptsize=ctsize;
//       ptsize= vec2(int(  ((Terrain*)ae->mesh)->sizex ),int(  ((Terrain*)ae->mesh)->sizey ));
  }
    ae->mesh->tile=tile->toVec();
    ae->mesh->tileoff=tileoff->toVec();
    
}

void MeshComp::sort()
{
  Entity* ae=EMenu::activeEntity;
  y=Inspector::y+Inspector::H;
  H=-20;

  label->setPos(Inspector::x+70,y+12.0);
  if(ae&&ae->mesh){
    if(ae->mesh->type==1)meshSelect->sort(H);
    if(ae->mesh->type==1&&((Mesh*)ae->mesh)->rawmesh->type&MESH_ANIM){
        animSelect->sort(H);
    }
    textureSelect->sort(H);
    if(ae->mesh->type==2){aspeed->sort(H);eframe->sort(H);sframe->sort(H);face->sort(H);}
    if(ae->mesh->type==3){for(int i=0;i<3;i++)tselects[i]->sort(H);tsize->sort(H);tscale->sort(H);}
    tile->sort(H);
    tileoff->sort(H);
    if(ae->mesh->type==1){
        smoothness->sort(H);
    }
    if (expand){
      doshadow->sort(H);
      transparent->sort(H);
      depthtest->sort(H);
      unshaded->sort(H);
    }
  }
  Inspector::H+=H;
}
MeshComp::~MeshComp()
{
    delete meshSelect;delete textureSelect;delete tile;delete tileoff;delete label;delete tselects[0];
    delete tselects[1];delete tselects[2];delete smoothness;delete face;
    delete transparent;delete aspeed;delete eframe;delete sframe;delete tsize;
    delete tscale;delete animSelect;delete depthtest;delete doshadow;delete unshaded;
  mco--;
  printf("((((((((((((((((((((((((((((((net num mcomps=%d \n",mco);
}
void MeshComp::remove()
{
    EMenu::activeEntity->mesh=0;
    this->empty=true;
    Inspector::sort();
}

///  Script Component  ///////////////////////////////
ScriptComp::ScriptComp()
{
  type=(4);
  scriptName=new Label("new_script",0,0,vec3(0.6,0.85,0.99));
  
  if(EMenu::activeEntity)get();
}

void ScriptComp::get()
{
 
  Entity* ae=EMenu::activeEntity;
  empty=(!ae || !ae->script);
      
  if(!empty)
  {
    scriptName->setText(EMenu::activeEntity->script->name);
    Script* script=EMenu::activeEntity->script;
    printf("name = %s number of props =%d \n\n",script->name.c_str(),script->props.size());
    for(int i=0;i<props.size();i++){delete props[i];}
    props.clear();
    scrprops.clear();
    for(int i=0;i<script->props.size();i++){
        ScriptProp *sp=script->props[i];
      if     (sp->type==1){
          Prop_Float *pf=new Prop_Float(sp->name,0);
          pf->ftext->setValue( ((SFloat*)sp)->value );
          props.push_back(pf);
    }
      else if(sp->type==2){
          Prop_Bool* pb=new Prop_Bool(sp->name,0);
          pb->button->toggle=((Sbool*)sp)->value;
          props.push_back(pb);
          
    }
      else if(sp->type==3){
          Prop_String *ps=new Prop_String(sp->name,((Sstring*)sp)->value);
          props.push_back(ps);
    }
      scrprops.push_back(sp);
    }
    //////
  }
}
void ScriptComp::sort()
{
    y=Inspector::y+Inspector::H;
    H=-30;short W=70;
    scriptName->setPos(Inspector::x+W,y+12.0);
    for(int i=0;i<props.size();i++){props[i]->sort(H);}
    Inspector::H+=H;
}
void ScriptComp::set()
{
  Entity* ae=EMenu::activeEntity;
  empty=(!ae || !ae->script);    
  if(!empty)
  {
    /// set script props /////
    for(int i=0;i<props.size();i++){
        short ttype=scrprops[i]->type;
        if     (ttype==1){
        ((SFloat*) scrprops[i])->value = ((Prop_Float*)props[i] )->ftext->value;

        }
        else if(ttype==2){
            ((Sbool*) scrprops[i])->value = ((Prop_Bool*)props[i] )->button->toggle;
        }
        else if(ttype==3){
            ((Sstring*) scrprops[i])->value = ((Prop_String*)props[i] )->field->label->text;
        }
    }
  }

}
void ScriptComp::tick()
{
  scriptName->tick();
  if(EMenu::activeEntity /*&& !Scene::activeScene->play*/)set();
  for(int i=0;i<props.size();i++){props[i]->tick();}
}
ScriptComp::~ScriptComp()
{
  delete scriptName;
  for(int i=0;i<props.size();i++){delete props[i];}
  props.clear();
}
void ScriptComp::remove()
{
    delete EMenu::activeEntity->script;
    EMenu::activeEntity->script=0;
    this->empty=true;
    Inspector::sort();
}

/// ////////////////////////////////////

SceneComp::SceneComp()
{
  type=(5);
  sceneSelect=new SelectionMenu<Scene>(-15.0-120,Shader::getResolution().y/2.0-24,0,Scene::scenes,true);
  sceneSelect->width=160.0f;
  sceneSelect->height=160.0f;
//   sceneSelect->sort();
//   sceneSelect->width=120;
  get();
}

void SceneComp::get()
{
    if(Scene::activeScene!=&Scene::def_scene)
        sceneSelect->setActive(Scene::activeScene->name);
    else sceneSelect->activeObj=&Scene::def_scene;
}

void SceneComp::set()
{
  /// switch scenes from scene menu
  if(sceneSelect->activeObj!=0 && Scene::activeScene!=sceneSelect->activeObj)
  {
//     if(Scene::activeScene==Scene::prefabs)
    {EMenu::activeEntity=0;}  
    EMenu::emenu->selected.clear();
    sceneSelect->activeObj->setActive();
    EMenu::emenu->reset();
    Inspector::refresh();
    
  }
}

void SceneComp::tick()
{
  if(!Scene::activeScene->play){
    sceneSelect->tick();
    if(!Scene::activeScene->play)set();
  }
}

void SceneComp::sort()
{
  y=Inspector::y+Inspector::H;
}
SceneComp::~SceneComp()
{
  delete sceneSelect;
}

/// ADD COMPONENT /////////////////////////////
bool jp;

AddComp::AddComp():active(0)
{
  type=(6);
  tbutton = new TextButton(0,0,"add Component",0);
  tbutton->bgColor=vec3(0.28,0.3,0.32)*0.6;
  tbutton->activeColor=vec3(0.0);
  cmenu   = new Menu(0,0,0);
  cmenu->visible=false;
  cmenu->addButton("Mesh");
  cmenu->addButton("Physics");
  cmenu->addButton("Script");
  cmenu->addButton("Light");
  cmenu->addButton("Sound");
  cmenu->addButton("UI");
  cmenu->addButton("GridMap");
  cmenu->addButton("Waypoint");
  cmenu->addButton("Camera");
  cmenu->addButton("Quad");
  cmenu->addButton("Terrain");
  cmenu->width=120 /*tbutton->transform.scale.x*/;
  
  meshMenu    = new Menu(0,0,0);
  scriptMenu  = new Menu(0,0,0);
  lightMenu   = new Menu(0,0,0);
  soundMenu   = new Menu(0,0,0);
  UIMenu      = new Menu(0,0,0);
  physicsMenu = new Menu(0,0,0);
  meshMenu->width=cmenu->width;
  scriptMenu->width=cmenu->width;
  lightMenu->width=cmenu->width;
  soundMenu->width=cmenu->width;
  UIMenu->width=cmenu->width;
  physicsMenu->width=cmenu->width;
  for(int a=0;a<RawMesh::meshes.size();a++){
    RawMesh* m=RawMesh::meshes[a];
    TextButton* tb=meshMenu->addButton(m->name);
    tb->bgColor=vec3(0.6,0.4,0.3);
  }
//   printf("kazam %d \n",Script::scripts.size());
  for(int a=0;a<Script::scripts.size();a++){
    TextButton* tb=scriptMenu->addButton(Script::scripts[a]->name);
    tb->bgColor=vec3(0.3,0.4,0.6);
  }
  lightMenu->addButton("Point Light");
  lightMenu->addButton("Directional Light");
  lightMenu->addButton("Spot Light");
  
  UIMenu->addButton("Button");
  UIMenu->addButton("Text Button");
  UIMenu->addButton("Label");
  UIMenu->addButton("Sprite");
  UIMenu->addButton("Slider");
 
  physicsMenu->addButton("Box Collider");
  physicsMenu->addButton("Capsule Collider");
  physicsMenu->addButton("Sphere Collider");
  physicsMenu->addButton("Convex Hull");
  physicsMenu->addButton("TriangleMesh");
  physicsMenu->addButton("Plane Collider");
  physicsMenu->addButton("Cylinder Collider");
  physicsMenu->addButton("Terrain Collider");
  physicsMenu->addButton("Empty Collider");
  for(TextButton* tbp:lightMenu->tbuttons){tbp->bgColor=vec3(0.6,0.5,0.4);}
  for(int p=0;p<physicsMenu->tbuttons.size();p++)
    {physicsMenu->tbuttons[p]->bgColor=vec3(0.43*0.9,0.63*0.9,0.33*0.9);}
    
  for(int p=0;p<UIMenu->tbuttons.size();p++)
    {UIMenu->tbuttons[p]->bgColor=vec3(0.9,0.8,0.2);}
 
  for(int a=0;a<Audio::audios.size();a++){
    TextButton* tb=soundMenu->addButton(Audio::audios[a]->name);
    tb->bgColor=vec3(0.4,0.4,0.7);
  }
}

void AddComp::sort()
{
  H=0;short W=50;
  tbutton->setPos(W+Inspector::x,Inspector::y+Inspector::H+H);
  cmenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);
  
  meshMenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);
  scriptMenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);
  lightMenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);
  soundMenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);
  UIMenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);
  physicsMenu->set(W+Inspector::x,Inspector::y+Inspector::H+H);  
  
  
  H=-40;
  Inspector::H+=H;
}

void AddComp::tick()
{
    if(!Scene::activeScene->play && EMenu::activeEntity){
      if(Input::getKeyDown(KEY_ESCAPE)||(!cmenu->hover()&&Input::getMouseButtonDown(0))){cmenu->visible=false;active=0;}
      tbutton->tick();
      if(tbutton->justDown && !active){cmenu->visible=!cmenu->visible;}
      cmenu->tick();
      /// //////////
      jp=false;
      if(cmenu->visible){
	  for(int a=0;a<cmenu->tbuttons.size();a++){
          TextButton* tb=cmenu->tbuttons[a];
	    if(tb->justDown){
	      if     (tb->label->text=="Mesh"){active=meshMenu;}
          else if(tb->label->text=="Quad"){setMeshComp(2);}
          else if(tb->label->text=="Terrain"){setMeshComp(3);}
	      else if(tb->label->text=="Script"){active=scriptMenu;}
	      else if(tb->label->text=="Light"){active=lightMenu;}
	      else if(tb->label->text=="Sound"){active=soundMenu;}
          else if(tb->label->text=="UI"){active=UIMenu;}
          else if(tb->label->text=="Physics"){active=physicsMenu;}
          else if(tb->label->text=="Waypoint" && EMenu::activeEntity->waypoint==0){
              EMenu::activeEntity->waypoint=new Waypoint();
              Scene::activeScene->bakeNavigation();
              Inspector::refresh();
          }
          else if(tb->label->text=="GridMap" && EMenu::activeEntity->gridmap==0){
              EMenu::activeEntity->gridmap=new GridMap();
              Inspector::refresh();
          }
	      else if(tb->label->text=="Camera" && EMenu::activeEntity->camera==0){
            Camera *tcam=new Camera();
            EMenu::activeEntity->camera=tcam;
            tcam->set();
            cc->get();
            Inspector::sort();
	      }
	      
	      cmenu->visible=false;
	      jp=true;break;
	    }
	    
	  }
      }
      if(active){
        active->tick();
        for(int a=0;a<active->tbuttons.size();a++)
        {
            TextButton*tb=active->tbuttons[a];
            if(tb->justDown && !jp){
                if(active==meshMenu && !jp){setMeshComp(1,tb->label->text);}
                else if(active==scriptMenu){setScriptComp(tb->label->text);}
                else if(active==lightMenu){setLightComp(tb->label->text);}
                else if(active==soundMenu){setSoundComp(tb->label->text);}
                else if(active==UIMenu){setUIComp(tb->label->text);}
                else if(active==physicsMenu){setPhysicsComp(tb->label->text);}
                active=0;break;
            }
        }
      }
    }
}
AddComp::~AddComp()
{
  delete tbutton;delete cmenu;delete meshMenu;delete soundMenu;
  delete lightMenu;delete scriptMenu;delete UIMenu;delete physicsMenu;
}
void AddComp::get(){}
void AddComp::set(){}

/// cam Component

camComp::camComp()
{
    type=56;
  title=new Label("Camera",0,0,vec3(0.99,0.6,0.4));
   near=new Prop_Float("near",0);
   far=new Prop_Float("far",0);
   fov=new Prop_Float("fov",0);
   size=new Prop_Float("size",0);
   isortho=new Prop_Bool("ortho",0);
   aspect_ratio=new Prop_Float("aspect_ratio",0);
}
void camComp::sort()
{
 if(EMenu::activeEntity){
    y=Inspector::y+Inspector::H;
    H=-20;
    title->setPos(Inspector::x+70,y+15);
    aspect_ratio->sort(H);
    near->sort(H);
    far->label->setPos(near->label->x+120.0,near->label->y);
    far->ftext->setPos(near->ftext->transform.position.x+90,near->ftext->transform.position.y);
    
    isortho->sort(H);
    if(EMenu::activeEntity->camera && EMenu::activeEntity->camera->ortho){size->sort(H);}
    else {fov->sort(H);}
    Inspector::H+=H;
 }
}


void camComp::get()
{
  empty=(EMenu::activeEntity==0 || EMenu::activeEntity->camera==0);
  if(!empty){
      Camera *camera=EMenu::activeEntity->camera;
      near->ftext->setValue(camera->near);
      far->ftext->setValue(camera->far);
      aspect_ratio->ftext->setValue(camera->aspect_ratio);
      isortho->button->toggle=camera->ortho;
      size->ftext->setValue(camera->size);
      fov->ftext->setValue(camera->fov);
      portho=camera->ortho;
  }
}

void camComp::set()
{
    Camera *camera=EMenu::activeEntity->camera;
    camera->ortho=isortho->button->toggle;
     
    camera->size=size->ftext->value;
    camera->fov=fov->ftext->value;
    
    camera->near=near->ftext->value;
     camera->far=far->ftext->value;
     camera->aspect_ratio= aspect_ratio->ftext->value;
     if(portho!=camera->ortho){
      if(camera->ortho)
          camera->proj=mat4::Orthographic(-16.0,16.0,-9.0,9.0,0.0,1000.0);
      else
          camera->proj=mat4::Perspective((60.0)*M_PI/180.0, 16.0f/9.0, 0.1, 1000.0);
      camera->set();
      Inspector::sort();
    }
     portho=camera->ortho;
}

void camComp::tick()
{
    if(!empty){
        title->tick();
        aspect_ratio->tick();
        near->tick();
        far->tick();
        isortho->tick();
        if(EMenu::activeEntity->camera && EMenu::activeEntity->camera->ortho){size->tick();}
        else {fov->tick();}
        set();
    }
}
camComp::~camComp()
{
  delete title;delete near;delete far;delete isortho;delete fov;delete size;delete aspect_ratio;
}
void camComp::remove()
{
    if(Scene::activeScene->camera==EMenu::activeEntity->camera)
    {Scene::activeScene->camera=0;}
    delete EMenu::activeEntity->camera;
    EMenu::activeEntity->camera=0;
    this->empty=true;
    Inspector::sort();
}


/// Light Component

LightComp::LightComp()
{
    title=new Label("Light",0,0,vec3(0.99,0.99,0.0));
    colour=new Prop_Vector("colour",vec3(1.0));
    radius=new Prop_Float("radius",20);
    intensity=new Prop_Float("intensity",0);
    shadow= new Prop_Bool("Do Shadow",0);
    
    colour->buttons[0]->tb->label->setText("R");
    colour->buttons[1]->tb->label->setText("G");
    colour->buttons[2]->tb->label->setText("B");
    intensity->ftext->clamp=2;radius->ftext->clamp=1;
}

void LightComp::sort()
{
    if(EMenu::activeEntity){
        y=Inspector::y+Inspector::H;
   Light* light=EMenu::activeEntity->light;
   H=-25;short W=50;
   title->setPos(Inspector::x+20.0+W,y+20.0);
   colour->sort(H);
   intensity->sort(H);
   shadow->sort(H);
    if(light && light->type==T_POINT){radius->sort(H);}

   Inspector::H+=H;
    }
}

void LightComp::get()
{
    empty=(EMenu::activeEntity==0 || EMenu::activeEntity->light==0);
    if(!empty){
        Light* light=EMenu::activeEntity->light;
        
        shadow->button->toggle=light->shadow;
        if(light) colour->fromVec(EMenu::activeEntity->light->colour);
        else{colour->fromVec(vec3(0.9f,0.9f,0.9f));}
        intensity->ftext->setValue(light->intensity);
        if (light && light->type==T_POINT){
            radius->ftext->setValue (light->radius);
        }
    }
}

void LightComp::set()
{
  Light* light=EMenu::activeEntity->light;
  if(light) 
      EMenu::activeEntity->light->colour=colour->toVec();
  light->intensity=intensity->ftext->value;
  if (light && light->type==T_POINT){
      light->radius=radius->ftext->value;
  }
  light->shadow=shadow->button->toggle;
}

void LightComp::tick()
{
  Light* light=EMenu::activeEntity->light;
  shadow->tick();
  if(!empty)set();
  title->tick();
  colour->tick();
  intensity->tick();
  if(light && light->type==T_POINT){radius->tick();}
  if(type==0){}
}

LightComp::~LightComp()
{
  delete title;delete colour;delete radius;delete intensity;delete shadow;
}

void LightComp::remove(){
    delete EMenu::activeEntity->light;
    EMenu::activeEntity->light=0;
    this->empty=true;
    Inspector::sort();
}
/// //////////////////////////////////


SoundComp::SoundComp()
{
    title=new Label("Sound",0,0,vec3(0.99,0.99,0.0));
    gain=new Prop_Float("gain",0);
    pitch=new Prop_Float("pitch",0);
    playonawake=new Prop_Bool("play on awake",0);
    positional=new Prop_Bool("positional",0);
    loop=new Prop_Bool("loop",0);
   audio=new Prop_Select<Audio>("audio",Audio::audios,true);
    gain->ftext->clamp=2;pitch->ftext->clamp=2;
}
void SoundComp::get()
{
    empty=(EMenu::activeEntity==0 || EMenu::activeEntity->sound==0);
    if(!empty){
        Sound* sound=EMenu::activeEntity->sound;
        pitch->ftext->setValue(sound->pitch);
        gain->ftext->setValue(sound->gain);
        loop->button->toggle=sound->flags&LOOP;
        positional->button->toggle=sound->flags&POSITIONAL;
        playonawake->button->toggle=sound->flags&PLAYONAWAKE;
        for(int u=0;u<audio->selMenu->tbuttons.size();u++)
        {
            ObjButton<Audio>* tmb=(ObjButton<Audio>*) audio->selMenu->tbuttons[u];
            if(tmb->object==sound->audio){
                audio->selMenu->activeObj=tmb->object;
                audio->selMenu->active=tmb;
                audio->selMenu->title->label->setText(tmb->label->text);
                break;
            }
        }
    }
}
void SoundComp::sort()
{
    y=Inspector::y+Inspector::H;
     H=-20;short W=70;
   title->setPos(Inspector::x+W,y+15);
    pitch->sort(H);
    gain->sort(H);
    loop->sort(H);
    playonawake->sort(H);
    audio->sort(H);
    positional->sort(H);
   Inspector::H+=H;
}
void SoundComp::set()
{
    Sound* sound=EMenu::activeEntity->sound;
    sound->pitch=pitch->ftext->value;
    sound->gain=gain->ftext->value;
    if(loop->button->toggle)sound->flags|=LOOP;
    else sound->flags&=~LOOP;
    if(positional->button->toggle)sound->flags|=POSITIONAL;
    else sound->flags&=~POSITIONAL;
    if(playonawake->button->toggle)sound->flags|=PLAYONAWAKE;
    else sound->flags&=~PLAYONAWAKE;
    EMenu::activeEntity->sound->audio=audio->selMenu->activeObj;
}
void SoundComp::tick()
{
    title->tick();
    pitch->tick();
    gain->tick();
    positional->tick();
    playonawake->tick();
    audio->tick();
    loop->tick();
    if(!empty)set();
}
SoundComp::~SoundComp()
{
    delete title;delete audio;delete positional;delete gain;delete pitch;delete playonawake;delete loop;
}
void SoundComp::remove()
{
    delete EMenu::activeEntity->sound;
    EMenu::activeEntity->sound=0;
    this->empty=true;
    Inspector::sort();
}

///

UIComp::UIComp()
{
    title=new Label("Button",0,0,vec3(0.99,0.99,0.0));
    fx=new Prop_Float("x",0);fy=new Prop_Float("y",0);
    fw=new Prop_Float("w",0);fh=new Prop_Float("h",0);

    font=new Prop_Select<Font>("font",Font::fonts,false);
    bgselect=new Prop_Select<Texture>("Background",Texture::textures);
    handleselect=new Prop_Select<Texture>("Handle",Texture::textures);
    fw->ftext->setValue(90.0);fh->ftext->setValue(30.0);
    bgcolour=new Prop_Vector("tint",vec3(0.18,0.2,2.2));
    tcolour=new Prop_Vector("text colour",vec3(0.9,0.9,0.6));
    utext=new Prop_String("Text","some text");
    fvalue=new  Prop_Float("value",0);
     alpha=new  Prop_Float("alpha",0);
    fontsize=new Prop_Float("font size",0);
    pad=new Prop_Vec2("Pad",vec2(0.0,0.0));
    pad->buttons[0]->clamp=1.0;
    pad->buttons[1]->clamp=1.0;
    empty=true;
}
void UIComp::get()
{
    empty=(EMenu::activeEntity==0 || EMenu::activeEntity->button==0);
    if(!empty){
        Button* b=EMenu::activeEntity->button;
        vec3 bp=b->transform.position,bs=b->transform.scale;
        fx->ftext->setValue(bp.x);fy->ftext->setValue(bp.y);
        fw->ftext->setValue(bs.x);fh->ftext->setValue(bs.y);
        if(b->texture){bgselect->selMenu->setActive(b->texture->name);}
        else {bgselect->selMenu->setActive("none");}
        if(b->type==1)
        {pad->fromVec(vec2(((TextButton*)b)->padX,((TextButton*)b)->padY));}
        if(b->type==1 || b->type==5){
            utext->field->label->setText(((TextButton*)b)->label->text);
            fontsize->ftext->setValue(((TextButton*)b)->label->scale);
            // past value
            ptext=utext->field->label->text;
            tcolour->fromVec(((TextButton*)b)->label->color);
            if(b->type==5)title->setText("label");
            else title->setText("TextButton");
            if(((TextButton*)b)->label->font!=Font::efonts[0]){
                font->selMenu->setActive(((TextButton*)b)->label->font->name);
            }
            else{
                font->selMenu->active=font->selMenu->none;
                font->selMenu->activeObj=0;
            }
        }
        else if(b->type==2){
            title->setText("slider");
            fvalue->ftext->setValue(((Slider*)b)->value);
            if(((Slider*)b)->handle->texture)
            {handleselect->selMenu->setActive(((Slider*)b)->handle->texture->name);}
        }
        else {
            if(b->type==6)title->setText("sprite");
            else title->setText("Button");
            bgcolour->fromVec(b->uColor);alpha->ftext->setValue(b->alpha);
        }
        pfsize=fontsize->ftext->value;
        ppad=pad->toVec();
    }
}

void UIComp::sort()
{
    y=Inspector::y+Inspector::H;H=-20.0;
    title->setPos(Inspector::x+80,y+15);
    fx->sort(H);
    float g=100.0;
    fy->label->setPos(fx->label->x+g,fx->label->y);
    fy->ftext->setPos(fx->ftext->transform.position.x+g-20,fx->ftext->transform.position.y);
    
    fw->sort(H);
    
    fh->label->setPos(fw->label->x+g,fw->label->y);
    fh->ftext->setPos(fw->ftext->transform.position.x+g-20,fw->ftext->transform.position.y);
    
    bgselect->sort(H);
    if(EMenu::activeEntity->button->type!=5&&EMenu::activeEntity->button->type!=2){
        bgcolour->sort(H);
        alpha->sort(H);
    }
    if(EMenu::activeEntity->button->type==1){pad->sort(H);}
    if(EMenu::activeEntity->button->type==1||EMenu::activeEntity->button->type==5)
    {utext->sort(H);fontsize->sort(H);tcolour->sort(H);font->sort(H);}
    if(EMenu::activeEntity->button->type==2){handleselect->sort(H);fvalue->sort(H);}
    
    Inspector::H+=H;
    
}

void UIComp::set()
{
    Button* b=EMenu::activeEntity->button;
    b->resize(fx->ftext->value,fy->ftext->value,fw->ftext->value,fh->ftext->value);
    b->texture=bgselect->selMenu->activeObj;
    b->uColor=bgcolour->toVec();
    b->alpha=alpha->ftext->value;
    if(b->type==1 || b->type==5){
        if(pfsize!=fontsize->ftext->value)
        {((TextButton*)b)->label->setScale(fontsize->ftext->value);}
        if(ptext!=utext->field->label->text)
        {
            ((TextButton*)b)->label->setText(utext->field->label->text);
            ((TextButton*)b)->setSize(((TextButton*)b)->label->lwidth+30.0,b->transform.scale.y);
        }
        ((TextButton*)b)->setPos(fx->ftext->value,fy->ftext->value);
        if(b->type==1 &&ppad!=pad->toVec())
        {
            ppad=pad->toVec();
            ((TextButton*)b)->padX=ppad.x;((TextButton*)b)->padY=ppad.y;
            ((TextButton*)b)->setPos(fx->ftext->value,fy->ftext->value);
        }
        ((TextButton*)b)->label->color=tcolour->toVec();
        if(b->type==1)((TextButton*)b)->uColor=bgcolour->toVec();
        // past values
        ptext=utext->field->label->text;
        if(font->selMenu->activeObj){
            ((TextButton*)b)->label->font=font->selMenu->activeObj;
            ((TextButton*)b)->label->setText(((TextButton*)b)->label->text);
        }
    }
    else if(b->type==2){
        ((Slider*)b)->value=(fvalue->ftext->value);
        ((Slider*)b)->handle->texture=handleselect->selMenu->activeObj;
        ((Slider*)b)->setPos(fx->ftext->value,fy->ftext->value);
    }
    pfsize=fontsize->ftext->value;
    ppad=pad->toVec();
//     if(EMenu::emenu->dragged)get();
}
void UIComp::tick()
{
    if(!empty){
        title->tick();
        fx->tick();
        fy->tick();
        fw->tick();
        fh->tick();
        if(EMenu::activeEntity->button->type!=5&&EMenu::activeEntity->button->type!=2)
        {bgcolour->tick();alpha->tick();}
        if(EMenu::activeEntity->button->type==1){pad->tick();}
        if(EMenu::activeEntity->button->type==1||EMenu::activeEntity->button->type==5)
            {fontsize->tick();utext->tick();tcolour->tick();font->tick();}
        if(EMenu::activeEntity->button->type==2){fvalue->tick();handleselect->tick();}
        bgselect->tick();
        if(!EMenu::emenu->dragged)set();
        else get();
    }
}
UIComp::~UIComp()
{
    delete fx;delete fy;delete fw;delete fh;delete title;delete fvalue;delete utext;
    delete fontsize;delete pad;delete tcolour;delete bgcolour;delete font;delete alpha;
    delete handleselect;delete bgselect;
}


void UIComp::remove()
{
    if(!empty)
    {delete EMenu::activeEntity->button;EMenu::activeEntity->button=0;}
    empty=true;
    Inspector::sort();
}

/// //////////////////////////////////////

PhysicsComp::PhysicsComp()
{
    empty=true;
    title=new Label("Physics",0,0,vec3(0.7,0.99,0.5));
    offset=new Prop_Vector("Offset",vec3(0.0));
    roffset=new Prop_Vector("Angular Offset",vec3(0.0));
    boxSize=new Prop_Vector("Half Size",vec3(0.0));
    afac=new Prop_Vector("Angular Factor",vec3(0.0));
    lifac=new Prop_Vector("Linear Factor",vec3(0.0));
    mass=new Prop_Float("mass",0);
    friction=new Prop_Float("friction",0);
    restitution=new Prop_Float("restitution",0);
    radius=new Prop_Float("radius",0);
    height=new Prop_Float("half height",0);
    trigger=new Prop_Bool("is Trigger",0);
    kinematic=new Prop_Bool("is Kinematic",0);
    istatic=new Prop_Bool("Static",0);
    nosleep=new Prop_Bool("Disable sleep",0);
    radius->ftext->clamp=1;
    height->ftext->clamp=1;
    mass->ftext->clamp=1;restitution->ftext->clamp=1;
    friction->ftext->clamp=1;
    for(int p=0;p<3;p++){
        lifac->buttons[p]->clamp=2;
        afac->buttons[p]->clamp =2;
    }
}

void PhysicsComp::get()
{
    empty=(EMenu::activeEntity==0 || EMenu::activeEntity->rigidBody==0);
    if(!empty){
        btRigidBody* rb= EMenu::activeEntity->rigidBody;
        btCollisionShape* shape=rb->getCollisionShape();
        int stype=shape->getShapeType();

        mass->ftext->setValue(1/rb->getInvMass());
        friction->ftext->setValue(rb->getFriction());
        restitution->ftext->setValue(rb->getRestitution());
        if(stype==BOX_SHAPE_PROXYTYPE||stype==CYLINDER_SHAPE_PROXYTYPE)
        {
            btVector3 tv;
             if(stype==BOX_SHAPE_PROXYTYPE)
             {tv=((btBoxShape*)shape)->getHalfExtentsWithMargin();title->setText("Box Collider");}
             else
             {tv=((btCylinderShape*)shape)->getHalfExtentsWithMargin();title->setText("Cylinder Collider");}
            boxSize->fromVec(vec3(tv.x(),tv.y(),tv.z()));
        }
        else if(stype==SPHERE_SHAPE_PROXYTYPE)
        {
            radius->ftext->setValue(((btSphereShape*)shape)->getRadius());
            title->setText("Sphere Collider");
        }
        else if(stype==CAPSULE_SHAPE_PROXYTYPE)
        {
            radius->ftext->setValue(((btCapsuleShape*)shape)->getRadius());
            height->ftext->setValue(((btCapsuleShape*)shape)->getHalfHeight());
            title->setText("Capsule Collider");
        }
        else if(stype==STATIC_PLANE_PROXYTYPE)
            title->setText("Plane Collider");
        else if(stype==TERRAIN_SHAPE_PROXYTYPE)
            title->setText("Terrain Collider");
        btVector3 offs=EMenu::activeEntity->motionState->m_centerOfMassOffset.getOrigin();
        btQuaternion roffs=EMenu::activeEntity->motionState->m_centerOfMassOffset.getRotation();
        offset->fromVec(toVec3(offs));
        roffset->fromVec(toQuat(roffs).ToEulerAngles()*180.0/PI);
        
        lifac->fromVec(toVec3(rb->getLinearFactor()));
        afac->fromVec(toVec3(rb->getAngularFactor()));
        trigger->button->toggle=(rb->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
        kinematic->button->toggle=(rb->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT);
        istatic->button->toggle=(rb->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT);
        nosleep->button->toggle=(rb->getCollisionFlags() & btCollisionObject::CF_CHARACTER_OBJECT);
        
        poffset=offset->toVec();
        proffset=roffset->toVec();
        pafac=afac->toVec();
        plifac=lifac->toVec();
        pboxsize=boxSize->toVec();
        pmass=mass->ftext->value;
        pfriction=friction->ftext->value;
        prestitution=restitution->ftext->value;
        pradius=radius->ftext->value;
        pheight=height->ftext->value;
        ptrigger=trigger->button->toggle;
        pkinematic=kinematic->button->toggle;
        pstatic=istatic->button->toggle;
        psleep=nosleep->button->toggle;
        pscale=EMenu::activeEntity->transform.scale;
    }
}

void PhysicsComp::sort()
{
    btCollisionShape* shape=EMenu::activeEntity->collShape;
    y=Inspector::y+Inspector::H;H=-20.0;
    title->setPos(Inspector::x+60,y+15);
    int stype=shape->getShapeType();
    istatic->sort(H);
    mass->sort(H);
    friction->sort(H);
    restitution->sort(H);
    if(stype==BOX_SHAPE_PROXYTYPE||stype==CYLINDER_SHAPE_PROXYTYPE)
    {
        boxSize->sort(H);
    }
    else if(stype==SPHERE_SHAPE_PROXYTYPE)
    {
        radius->sort(H);
    }
    else if(stype==CAPSULE_SHAPE_PROXYTYPE)
    {
        radius->sort(H);
        height->sort(H);
    }
    if(expand){
        trigger->sort(H);
        nosleep->sort(H);
        kinematic->sort(H);
        
        offset->sort(H);
        roffset->sort(H);
        afac->sort(H);
        lifac->sort(H); 
    }
    
    Inspector::H+=H;
}

void PhysicsComp::set()
{
        btRigidBody* rb= EMenu::activeEntity->rigidBody;
        btCollisionShape* shape=EMenu::activeEntity->collShape;
        Entity* ent =EMenu::activeEntity;
        int stype=shape->getShapeType();
        quat rot=ent->rtrans.rotation;vec3 pos=ent->rtrans.position;
        if((stype==BOX_SHAPE_PROXYTYPE||stype==CYLINDER_SHAPE_PROXYTYPE)&& pboxsize!=boxSize->toVec())
        {
                delete ent->collShape;
                pboxsize=boxSize->toVec();
                btVector3 bhs(pboxsize.x,pboxsize.y,pboxsize.z);
                if(stype==BOX_SHAPE_PROXYTYPE)ent->collShape=new btBoxShape(bhs);
                else       ent->collShape=new btCylinderShape(bhs);
                rb->setCollisionShape(ent->collShape);
        }
        else if(stype==SPHERE_SHAPE_PROXYTYPE && pradius!=radius->ftext->value)
        {
                delete ent->collShape;
                ent->collShape=new btSphereShape(radius->ftext->value);
                rb->setCollisionShape(ent->collShape);
        }
        else if(stype==CAPSULE_SHAPE_PROXYTYPE &&
            (pradius!=radius->ftext->value||pheight!=height->ftext->value))
        {
                delete ent->collShape;
                ent->collShape=new btCapsuleShape(radius->ftext->value,height->ftext->value*2.0);
                rb->setCollisionShape(ent->collShape);
        }
        
        if(poffset!=offset->toVec()){
            poffset=offset->toVec();
            ent->motionState->m_centerOfMassOffset.setOrigin(toBVec3(poffset));
        }
        if(proffset!=roffset->toVec()){
            proffset=roffset->toVec();
            ent->motionState->m_centerOfMassOffset.
            setRotation(toBQuat(quat::FromEulerAngles(proffset*PI/180.0)));
        }
        
        if(plifac!=lifac->toVec()){
            plifac=lifac->toVec();
            rb->setLinearFactor(btVector3(plifac.x,plifac.y,plifac.z));
        }
        if(pafac!=afac->toVec()){
            pafac=afac->toVec();
            rb->setAngularFactor(btVector3(pafac.x,pafac.y,pafac.z));
        }
        if(pmass!=mass->ftext->value){
            // set mass and fall inertia
            btVector3 fallInertia(0.0,0.0,0.0);
            ent->collShape->calculateLocalInertia(mass->ftext->value, fallInertia);
            rb->setMassProps(mass->ftext->value,fallInertia);
        }
        if(pfriction!=friction->ftext->value){
            rb->setFriction(friction->ftext->value);
        }
        if(prestitution!=restitution->ftext->value){
            rb->setRestitution(restitution->ftext->value);
        }
        if(ptrigger!=trigger->button->toggle){
            ptrigger=trigger->button->toggle;
            rb->setCollisionFlags(rb->getCollisionFlags() ^ btCollisionObject::CF_NO_CONTACT_RESPONSE);//toggle
        }
        if(pkinematic!=kinematic->button->toggle){
            pkinematic=kinematic->button->toggle;
            rb->setCollisionFlags(rb->getCollisionFlags() ^ btCollisionObject::CF_KINEMATIC_OBJECT);
        }
        if(pstatic!=istatic->button->toggle){
            pstatic=istatic->button->toggle;
            rb->setCollisionFlags(rb->getCollisionFlags() ^ btCollisionObject::CF_STATIC_OBJECT);
        }
        if(psleep!=nosleep->button->toggle){
            psleep=nosleep->button->toggle;
            rb->setCollisionFlags(rb->getCollisionFlags() ^ btCollisionObject::CF_CHARACTER_OBJECT);
//             if(psleep)rb->setActivationState(DISABLE_DEACTIVATION);
//             else {rb->forceActivationState(ACTIVE_TAG);printf("fuuuukuu b\n");}
        }
        
        // scale box,cylinder and capsule with transform scale
        if(pscale!=EMenu::activeEntity->rtrans.scale){
            for(int l=0;l<3;l++){if(pscale[l]==0)pscale[l]=0.01;}
            vec3 tsv=EMenu::activeEntity->rtrans.scale/pscale;
            for(int l=0;l<3;l++){if(tsv[l]==0)tsv[l]=1.0;}
            
            //scaling half-extents
            btVector3 sv=toBVec3(tsv);
            btVector3 bhs=sv*toBVec3(boxSize->toVec());
            //scaling offset
            btVector3 ov(poffset.x,poffset.y,poffset.z);
            ent->motionState->m_centerOfMassOffset.setOrigin(sv*ov);
            poffset=poffset*toVec3(sv);offset->fromVec(poffset);
            boxSize->fromVec(toVec3(bhs));         
            if(stype==BOX_SHAPE_PROXYTYPE){
                ent->collShape->setLocalScaling(sv*ent->collShape->getLocalScaling());
                boxSize->fromVec(toVec3(   ((btBoxShape*)ent->collShape)->getHalfExtentsWithMargin() ));
                /*
                delete ent->collShape;
                ent->collShape=new btBoxShape(bhs);
                rb->setCollisionShape(ent->collShape);
                */
            }
            else if(stype==CYLINDER_SHAPE_PROXYTYPE){
                ent->collShape->setLocalScaling(sv*ent->collShape->getLocalScaling());
                radius->ftext->setValue(((btSphereShape*)ent->collShape)->getRadius());
                /*
                delete ent->collShape;
                ent->collShape=new btCylinderShape(bhs);
                rb->setCollisionShape(ent->collShape);
                */
            }
            else if(stype==CAPSULE_SHAPE_PROXYTYPE){
                ent->collShape->setLocalScaling(sv*ent->collShape->getLocalScaling());
                radius->ftext->setValue(((btCapsuleShape*)ent->collShape)->getRadius());
                height->ftext->setValue(((btCapsuleShape*)ent->collShape)->getHalfHeight());
            }
            else if(stype==CONVEX_HULL_SHAPE_PROXYTYPE){
                ent->collShape->setLocalScaling(toBVec3(ent->rtrans.scale));
            }
        }
        
        poffset=offset->toVec();
        proffset=roffset->toVec();
        pafac=afac->toVec();
        plifac=lifac->toVec();
        pboxsize=boxSize->toVec();
        pmass=mass->ftext->value;
        pfriction=friction->ftext->value;
        prestitution=restitution->ftext->value;
        pradius=radius->ftext->value;
        pheight=height->ftext->value;
        ptrigger=trigger->button->toggle;
        pkinematic=kinematic->button->toggle;
        pstatic=istatic->button->toggle;
        psleep=nosleep->button->toggle;
        pscale=EMenu::activeEntity->rtrans.scale;
}
void PhysicsComp::tick()
{
    if(!empty){
        btCollisionShape* shape=EMenu::activeEntity->collShape;
        istatic->tick();
        title->tick();
        if(expand){
            offset->tick();
            roffset->tick();
            afac->tick();
            lifac->tick();
            trigger->tick();
            kinematic->tick();
            nosleep->tick();
        }
        mass->tick();
        friction->tick();
        restitution->tick();
        if(shape->getShapeType()==BOX_SHAPE_PROXYTYPE||shape->getShapeType()==CYLINDER_SHAPE_PROXYTYPE)
        {
            boxSize->tick();
        }
        else if(shape->getShapeType()==SPHERE_SHAPE_PROXYTYPE)
        {
            radius->tick();
        }
        else if(shape->getShapeType()==CAPSULE_SHAPE_PROXYTYPE)
        {
            radius->tick();
            height->tick();
        }
        if(!Scene::activeScene->play)set();
    }
}
PhysicsComp::~PhysicsComp()
{
    delete mass;delete radius;delete height;
    delete offset;delete boxSize;delete title;delete restitution;
    ;delete trigger;delete lifac;delete afac;delete roffset;delete friction;
    delete kinematic;delete nosleep;delete istatic;
}
void PhysicsComp::remove()
{
    Entity* ent =EMenu::activeEntity;
    ent->removeComp(1);
    empty=true;
    Inspector::sort();
}


WPcomp::WPcomp()
{
    title=new Label("Waypoint",0,0,vec3(0.7,0.99,0.5));
}

void WPcomp::get()
{
    empty=(EMenu::activeEntity==0 || EMenu::activeEntity->waypoint==0);
    if(!empty){
        
    }
}

void WPcomp::set()
{
}
void WPcomp::sort()
{
   if(EMenu::activeEntity){
        y=Inspector::y+Inspector::H;
        H=-30;short W=50;
        title->setPos(Inspector::x+20.0+W,y+10);
        Inspector::H+=H;
    }
}
void WPcomp::tick()
{
    title->tick();
    if(!empty)set();
}

void WPcomp::remove()
{
    delete EMenu::activeEntity->waypoint;
    EMenu::activeEntity->waypoint=0;
    this->empty=true;
    Scene::activeScene->bakeNavigation();
    Inspector::sort();
}


WPcomp::~WPcomp()
{
     delete title;
}
////////////////////////////    Grid Map      ///////////////////////

GMcomp::GMcomp()
{
    title=new Label("GridMap",0,0,vec3(0.7,0.99,0.5));
    dim=new Prop_Vector("dimensions",vec3(1));
    size=new Prop_Float("size",false);
    size->ftext->clamp=4;
}

void GMcomp::get()
{
    empty=(EMenu::activeEntity==0 || EMenu::activeEntity->gridmap==0);
    if(!empty){
        size->ftext->setValue(EMenu::activeEntity->gridmap->sizex);
        dim->fromVec(EMenu::activeEntity->gridmap->dimensions);
    }
    psize=size->ftext->value;
    pdim=dim->toVec();
}

void GMcomp::set()
{
    int tsize=size->ftext->value;
    vec3 tdim=dim->toVec();
    GridMap* gm=EMenu::activeEntity->gridmap;
    if(tdim!=pdim){
        gm->dimensions=tdim;
        gm->refresh(EMenu::activeEntity);
    }
    if(tsize!=psize && tsize>0){
        gm->sizex=gm->sizey=tsize;
        delete []gm->tiles; 
        gm->tiles=new unsigned char[tsize*tsize*2];
        memset(gm->tiles,255,tsize*tsize);
        memset(gm->tiles+tsize*tsize,0,tsize*tsize);
        gm->refresh(EMenu::activeEntity);
    }
    psize=tsize;
    pdim=tdim;
}
void GMcomp::sort()
{
   if(EMenu::activeEntity){
        y=Inspector::y+Inspector::H;
        H=-30;short W=50;
        title->setPos(Inspector::x+20.0+W,y+10);
        dim->sort(H);
        size->sort(H);
        Inspector::H+=H;
    }
}
void GMcomp::tick()
{
    title->tick();
    dim->tick();
    size->tick();
    if(!empty)set();
}

void GMcomp::remove()
{
    delete EMenu::activeEntity->gridmap;
    EMenu::activeEntity->gridmap=0;
    this->empty=true;
    Scene::activeScene->bakeNavigation();
    Inspector::sort();
}


GMcomp::~GMcomp()
{
     delete title;delete size;delete dim;
}

