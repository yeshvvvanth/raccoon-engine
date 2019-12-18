#include"entitymenu.h"

Entity* EMenu::activeEntity=0;
Scene* EMenu::pactiveScene=0;
int EMenu::entsize=0;
std::vector<Component*> Inspector::Components;
std::string EMenu::pscnname="";
EMenu* EMenu::emenu=0;
TextButton *newEnt,*dupe,*copy,*paste,*delEnt;
template <class T>
bool vremove(std::vector<T*>& vec,T* obj){
    typename std::vector<T*>::const_iterator iter;
    for(iter = vec.begin(); iter != vec.end(); ++iter )
    {
      if( *iter == obj )
      {
	  vec.erase( iter );
	  return 1;
	  break;
      }
    }
    return 0;
}
EMenu::EMenu(Font* fnt): Menu(-Shader::sc_res.x/2,Shader::sc_res.y/2, fnt),inspector(0)
{
  dragged=false;
  width=240.0;
  height=Shader::sc_res.y*0.85;
  sp=Sprite::sp;
  sbw=-12.0;
  reset();
  islist= true;
  scrollable=true;
  heir=true;
  cscene=0;
  ih=0;dy=0;
  EMenu::emenu=this;
  if(Scene::activeScene->entities.size()){
      activeEntity=Scene::activeScene->entities[0];
  }
  inspector= new Inspector();

  cmenu=new ContextMenu(0.0,0.0,0);
  newEnt=cmenu->addButton("Add Empty");
  dupe=cmenu->addButton("Duplicate");
  copy=cmenu->addButton("Copy");
  paste=cmenu->addButton("Paste");
  delEnt=cmenu->addButton("Delete");
  for(TextButton* tbp:cmenu->tbuttons){tbp->bgColor=vec3(0.4,0.4,0.9);}
  sbar.set(x+width+4.0,y-4.0,ih,height);// setting to avoid conditional jump 
}

EMenu::~EMenu()
{
  delete inspector;delete cmenu;
}

bool EMenu::addable(Entity* tent){
    if(tent->parent&&(tent->parent->gridmap||tent->parent->source.length())){return false;}
    else if(tent->parent)return addable(tent->parent);
    return true;
}

void EMenu::reset()
{
  printf("Emenu reset \n");
  sbar.dy=0;dy=0;
  Entity* aae=activeEntity,*te=0,*ae=0;
  std::vector<Entity*>sellbutts;
  for(int i=0;i<selected.size();i++){
      sellbutts.push_back(((ObjButton<Entity>*)selected[i])->object);
  }
  for(int i=0;i<tbuttons.size();i++){
    delete tbuttons[i];
  }
  tbuttons.clear();
  selected.clear();
  entsize=0;
//   printf("Adding Ebuttons:\n");
  for(int i=0;i<Scene::activeScene->entities.size();i++){
      Entity* tent=Scene::activeScene->entities[i];
      if(EMenu::addable(tent)){
//         printf("%d    %s\n",entsize,tent->name.c_str());
        addButton(Scene::activeScene->entities[i]);
        entsize++;
      }
  }
  if(sellbutts.size()){
      active= tbuttons[0];
      for(int i=0;i<sellbutts.size();i++){
          ae=sellbutts[i];
        for(int j=0;j<tbuttons.size();j++){
            te=((ObjButton<Entity>*)tbuttons[j])->object;
            if(ae==te){selected.push_back(tbuttons[j]);if(ae==aae)active=tbuttons[j];break;}
        }
      }
      activeEntity=aae;
  }
  else if(Scene::activeScene->entities.size()){
      activeEntity=Scene::activeScene->entities[0];
      active= tbuttons[0];
      selected.push_back(active);
      active->toggle=true;
  }
  else{
      activeEntity=0;
      active=0;
  }
  if(pactiveScene!=Scene::activeScene){
    activeEntity=0;
    active=0;
    selected.clear();
  }
  sort();
}


ObjButton<Entity>* EMenu::addButton(Entity* entity)
{
  std::string ent_name=entity!=0?entity->name:"empty";
  ObjButton<Entity>* but=new ObjButton<Entity>(x,y,ent_name,entity,font);
  but->isToggle=true;

  float& butwidth=but->transform.scale.x,&butheight=but->transform.scale.y;

//   width=butwidth>width?butwidth:width;
  ih=butheight>ih?butheight:ih;

  sp->resize(sp->transform.position.x,sp->transform.position.y,width,-Shader::sc_res.y);

  but->setPos(x,y+dy-(tbuttons.size()+1)*butheight,width,butheight);
  tbuttons.push_back(but);


  return but;
}
bool isExpand(TextButton* tb){
    ObjButton<Entity>* ob= ((ObjButton<Entity>*)tb);
    if(!ob->expand)return false;
    if(ob->object->parent)isExpand(EMenu::emenu->find(ob->object->parent));
    else    return true;
}
void EMenu::tick()
{
    float goff=scroll?-16:0;
  if(tbuttons.size()!=entsize&&!Scene::play)
  {reset();Inspector::refresh();}
    for(TextButton* but:tbuttons){
        ObjButton<Entity> *pb=0;
        Entity* tent=((ObjButton<Entity>*)but)->object;
        pb=EMenu::emenu->find(tent->parent);
        // visibility
        but->visible=((!pb || isExpand(pb)) );
  }
  Menu::tick();
  cmenuUpdate();
  for(TextButton* but:tbuttons){
    Entity* tent=( (ObjButton<Entity>*)but )->object;
    but->visible=but->toggle=false;
    if(but->label->text!=tent->name)
        {but->label->setText(tent->name);}
    if(but->justDown){
      if(!Input::getKey(KEY_LCTRL)&&!Input::getKey(KEY_LSHIFT)){selected.clear();}
      // make sure of no double selections
      bool nex=true;
      for(int a=0;a<selected.size();a++){
          if(but==selected[a]){nex=false;break;}
      }
      if(nex)selected.push_back(but);
      // end
      active=but;
      activeEntity=tent;
      but->toggle=true;
      inspector->refresh();
    }
   
    // select all entities if ctrl+a is pressed
    if(Input::getKeyDown(KEY_LCTRL,KEY_A)){selected=tbuttons;}
    if(tent->gridmap)
        but->activeColor=vec3(0.7,0.7,0.99);
    else
        but->activeColor=vec3(0.4,0.7,0.9);
    if(activeEntity&&tent==activeEntity){active=but;but->toggle=true;}
    ObjButton<Entity> *pb=0,*ob=0;
    {
    ob=((ObjButton<Entity>*)but);
    pb=EMenu::emenu->find(((ObjButton<Entity>*)but)->object->parent);
//     if(ob->over && Input::getMouseButtonDown(2)){ob->expand=!ob->expand;sort();}
    // visibility
    but->visible=((!pb || isExpand(pb))&&(!tent->parent||tent->parent->source.length()==0) );
    bool vis=but->transform.position.y>y-height;
    Button butt(0,0,16,16);
    vec3 tps=but->transform.position;
    if (but->visible&&vis){
        butt.setPos(x+width-16+goff,tps.y+4);
        if( tent->enabled){
            butt.texture=Texture::findTexture("visible.png",1);
        }
        else{
            butt.texture=Texture::findTexture("hidden.png",1);
        }
        butt.tick();
        if(butt.justDown){
            tent->enabled=!tent->enabled;emenu->dragged=true;
        }
    }
    // button expand option
    if(but->visible&& ob->object->children.size() && !ob->object->source.length()&&vis && !ob->object->gridmap ){
        
        butt.texture=Texture::findTexture("expand.png",1);
        butt.setPos(but->transform.position.x+but->padX-24,but->transform.position.y+4);
        if (ob->expand)butt.transform.rotation=quat::FromEulerAngles(vec3(0,0,-1.57));
        butt.tick();
        if (butt.down){ob->expand=!ob->expand;sort();}    
    }
    // show instantiate symbol
    if(tent->source.length()&&but->visible&&vis){
//         Sprite::render(x+width-16*2.0,tps.y+4,16,16,vec3(1.0),1.0,Texture::findTexture("inst.png",1));
        butt.transform.rotation=quat();
        butt.setPos(x+width-16*2+goff,tps.y+4);
        butt.texture=Texture::findTexture("inst.png",1);
        butt.tick();
        if(butt.justDown){
            if(Scene::activeScene!=Scene::prefabs||Scene::activeScene!=Scene::common)
                EMenu::pscnname=Scene::activeScene->name;
            inspector->scomp->sceneSelect->setActive("_prefabs_");
        }
    }
    if(tent->source.length()){but->bgColor=vec3(0.08,0.16,0.24);}
    else if(tent->gridmap){but->bgColor=vec3(0.7,0.3,0.3);but->bgColor=vec3(0.99,0.6,0.5)*0.9;}
    else but->bgColor=vec3(0.18,0.2,0.22);

    }
  }   
  for(TextButton* t:selected){
      t->toggle=true;
  }
  // making sure active button is selected
  if(active){
    bool nex=true;
    for(int a=0;a<selected.size();a++){
        if(active==selected[a]){nex=false;break;}
    }
    if(nex){selected.push_back(active);}
  }

  //end
  inspector->tick();
  for(TextButton* tbp:tbuttons){tbp->pdown=tbp->down;}
  pactiveScene=Scene::activeScene;  
}
void EMenu::repos(TextButton* tb, char pos)
{
    printf(" reposition Emenu :D \n\n");
    std::vector<Entity*>& entities=Scene::activeScene->entities;
    short a=-1,b=-1;
    Entity *te1=((ObjButton<Entity>*)tb)->object;
    Entity *te2=activeEntity=((ObjButton<Entity>*)active)->object;
    bool inst=(te1->source.length());
    for(int p=0;p<entities.size();p++){
       if(a<0 && te1==entities[p] ){a=p;}
       if(b<0 && te2==entities[p] ){b=p;}
    }

    bool notpar=true;
    Entity *par=te1->parent;
    while(par!=0){
        if(par==te2){notpar=false;break;}
        par=par->parent;
    }

    dragged=true;// pos==1 up pos==0 down pos==2 make child
    if((!te1->parent||!te2->parent)&&(pos==0||pos==1) && (a>-1&&b>-1) && notpar){// move up or down
        entities.erase(entities.begin()+b);
        entities.insert(entities.begin()+a,activeEntity);
        if(te2->parent!=te1->parent){
            if(te2->parent)te2->unparent();
            if(te1->parent){// if moved to list of children
                te1->parent->addChild(te2); // check it !
                std::vector<Entity*> &children=te1->parent->children;
                a=-1;b=children.size()-1;
                for(int p=0;p<children.size();p++){
                    if(a<0 && te1==children[p] ){a=p;}
                }
                if (a>-1){
                    if(pos==0)a++;
                    children.erase(children.begin()+b);
                    children.insert(children.begin()+a,activeEntity);
                    sort();
                }
            }
        }
        sort();
    }
    else if(pos==2 && notpar && te2->parent!=te1 ){// make child
        if(inst)return;
        te2->unparent();
        te1->addChild(te2);
        EMenu::dragged=true;
        sort();
    }
    else if(te1->parent==te2->parent!=0 && notpar){// move child up or down
        std::vector<Entity*> &children=te1->parent->children;a=b=-1;
        for(int p=0;p<children.size();p++){
            if(a<0 && te1==children[p] ){a=p;}
            if(b<0 && te2==children[p] ){b=p;}
        }
        if (a>-1&&b>-1){
            children.erase(children.begin()+b);
            children.insert(children.begin()+a,activeEntity);
            printf("______________________________child moved from %d to %d \n\n",a,b);
            sort();
        }
    }

}
void esort(TextButton* tb,short &h,EMenu* em,float twidth,float tbheight){

    Entity *en=((ObjButton<Entity>*)tb )->object;
    tb->setPos(em->x+em->pad,em->y-em->pad+em->dy-(h+1)*tbheight,twidth,tbheight);
    h++;
    printf(" esort name = %s h=%d\n",en->name.c_str(),h);
    for(int a=0;a<en->children.size();a++){
        TextButton* tb2=em->find(en->children[a]);
        if(tb2 && ((ObjButton<Entity>*)tb)->expand && (!en->source.length()))esort(tb2,h,em,twidth,tbheight);
    }

}
void EMenu::sort()
{
  float twidth= (scroll)?(width+sbw):width;
  for(int i=0;i<tbuttons.size();i++){
      Entity* e= ((ObjButton<Entity>*)tbuttons[i] )->object;
      TextButton* tb=tbuttons[i];

      tb->padX=25.0f;tb->Dim=1.0f;
      // children padd
      short u=0;
      Entity *par=e->parent;
      while(par!=0){
          par=par->parent;
          u++;
      }
      tb->padX+=u*PADX*3;
//       tb->setPos(x,y+dy-(i+1)*tbheight,twidth,tbheight);
  }
  short Htt=0;
  float tbheight;
   for(int i=0;i<tbuttons.size();i++){
      Entity* e= ((ObjButton<Entity>*)tbuttons[i] )->object;
      TextButton* tb=tbuttons[i];
      tbheight=tb->transform.scale.y;
      // children pos
      if(!e->parent)esort(tb,Htt,this,twidth,tbheight);
    }

    ih=Htt*tbheight+50;
    if(sbar.dy>ih-height)sbar.dy=ih-height;
    if(sbar.dy<0)sbar.dy=0;
    sbar.set(x+width+4.0,y-4.0,ih,height);
}
bool isInVec(std::vector<Entity*>& ents,Entity *tnt){
    int siz=ents.size();
    for(int i=0;i<siz;i++)
        if(ents[i]==tnt)return true;
    return false;
}
void EMenu::cmenuUpdate()
{
  sp->resize(x,y,width,-height);
  vec2 mp=Shader::scrMousePos();
  vec3 smp=sp->transform.position;
  float w=sp->transform.scale.x,h=sp->transform.scale.y;
  bool mip=mp.x>smp.x&& mp.x<smp.x+w && mp.y<smp.y && mp.y>smp.y+h;
  if(Input::getMouseButtonDown(1)&&mip){cmenu->visible=true;}

  if(newEnt->justDown  )
  {
      Entity* nent=new Entity(Scene::activeScene->newname("entity"));
      Scene::activeScene->entities.push_back(nent);
      active=addButton(nent);
      activeEntity=nent;
      selected.clear();
      selected.push_back(active);
      reset();
      nent->transform.position=Camera::active_camera->pos+Camera::active_camera->dir*10.0;
      inspector->refresh();
  }
  if(delEnt->justDown || Input::getKeyDown(KEY_DELETE)){
      for(int i=0;i<selected.size();i++){
        Entity* dent=((ObjButton<Entity>*)selected[i])->object;
         printf("\n +++++++++++ delete entity name= %s ind==%d +++++++++++++++\n",dent->name.c_str(),i);
        Entity::remove(dent);
      }
      selected.clear();
      reset();
      inspector->refresh();

    //
  }
  else if(dupe->justDown || (Input::getKeyDown(KEY_LCTRL,KEY_D))){
      bool hasgridmap=false;
      std::vector<TextButton*> sellbutts;
      for(int i=0;i<selected.size();i++){
            Entity* nent=((ObjButton<Entity>*)selected[i])->object->clone();
            nent->name=Scene::activeScene->newname(nent->name);
            nent->transform.position+=quat::Rotate(quat::FromEulerAngles(vec3(0.0,Input::time,0.0)), vec3(1,0,0))*0.4;
        active=addButton(nent);
        activeEntity=nent;
        sellbutts.push_back(active);
        active->toggle=true;
        if(nent->gridmap)hasgridmap=true;
      }
      selected=sellbutts;
      if(hasgridmap)Scene::activeScene->setActive();
      reset();//sort();
      inspector->refresh();
  }
  else if(copy->justDown || Input::getKeyDown(KEY_LCTRL,KEY_C)){
      if(!selected.empty()){
        copied.clear();
        copied.reserve(selected.size());
        for(int i=0;i<selected.size();i++){
            copied.push_back(((ObjButton<Entity>*)selected[i])->object);
        }
        cscene=Scene::activeScene;
        printf("entities copied \n");
      }
  }
  else if((paste->justDown || Input::getKeyDown(KEY_LCTRL,KEY_V))&&cscene){
      bool hasgridmap=false;
      for(int i=0;i<copied.size();i++){
          Entity *ent=0,*tent,*cent=copied[i];
          for(int j=0;j<cscene->entities.size();j++){
              tent=cscene->entities[j];
              if(cent==tent){ent=tent;break;}
          }
          if(ent){
            if(ent->gridmap)hasgridmap=true;
            if(!isInVec(copied,ent->parent))ent->parent=0;
            Entity* fresh=ent->clone();
            if(cscene==Scene::prefabs && !ent->parent)fresh->source=ent->name;
            printf("entities pasted\n");
          }
      }
      if(cscene==Scene::prefabs){
             Scene::activeScene->setActive();
      }
      if(hasgridmap)Scene::activeScene->setActive();
      reset();
      inspector->refresh();
  }
  cmenu->tick();
  if(active &&!Scene::play&& (Input::getKeyDown(KEY_UP)||Input::getKeyDown(KEY_DOWN))){
        int ky=-1;
      for(int i=0;i<tbuttons.size();i++){if(active==tbuttons[i]){ky=i;break;}}
      selected.clear();
      if(ky>-1&&Input::getKeyDown(KEY_DOWN)&& ky<tbuttons.size()-1){active=tbuttons[ky+1];}
      if(ky>0&&Input::getKeyDown(KEY_UP)){active=tbuttons[ky-1];}
      selected.push_back(active);
      activeEntity=((ObjButton<Entity>*)active)->object;
      inspector->refresh();
  }
}
ObjButton<Entity> * EMenu::find(Entity* ent)
{
    ObjButton<Entity> *e=0,*t=0;
    for(int i=0;i<tbuttons.size();i++){
        t=((ObjButton<Entity>*)tbuttons[i]);
        if(t->object==ent){e=t;return e;}
    }
    return 0;
}

/// //////////////////////////////
template<class T>
EList<T>::EList(float X, float Y, Font* fnt, std::vector<T *>& vec): Menu(X, Y, fnt),activeObj(0)
{
    islist=true;
    tvec=&vec;
    for(int i=0;i<vec.size();i++){
        addButton(vec[i]);
    }
}
template<class T>
ObjButton<T> * EList<T>::addButton(T* obj)
{
   ObjButton<T>* but =new ObjButton<T>(x,y,obj->name,obj,0);
   but->isToggle=true;
   tbuttons.push_back(but);
   Menu::sort();
   return but;
}
template<class T>
void EList<T>::refresh()
{
    for(int i=0;i<tbuttons.size();i++){delete tbuttons[i];}
    tbuttons.clear();
    for(int i=0;i<tvec->size();i++){addButton((*tvec)[i]);}
    sort();
}
template<class T>
void EList<T>::tick()
{
    if(tbuttons.size()!=(*tvec).size()){
      refresh();
    }
    Menu::tick();
    if(active &&   ((ObjButton<T>*)active)->object!=activeObj   )
    activeObj=((ObjButton<T>*)active)->object;

}

template<class T>
void EList<T>::repos(TextButton* tb, char pos)
{
    std::vector<T*>* tees= tvec;
    short a=-1,b=-1;
    T *te1=((ObjButton<T>*)tb)->object;
    T *te2=((ObjButton<T>*)active)->object;
    for(int p=0;p<tees->size();p++){
       if(a<0 && te1==(*tees)[p] ){a=p;}
       if(b<0 && te2==(*tees)[p] ){b=p;}
    }
    if((pos==0||pos==1) && (a>-1&&b>-1)){
        tees->erase(tees->begin()+b);
        tees->insert(tees->begin()+a,activeObj);
        sort();
    }
}

template<class T>
EList<T>::~EList()
{
}
template class EList<Scene>;
