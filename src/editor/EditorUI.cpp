#include"EditorUI.h"
#include<main/input.h>
  Sprite* sp=0; EMenu *emenu=0;
 Ewindow* EditorUI::awindow=0;
Label* EditorUI::actile=0;
Console::Console()
{
    float x=-Shader::getResolution().x/2.0+20.0;
    float y=-Shader::getResolution().y/2.0+20.0;
    lines=1;
    error=new Label("No problems.",x,y,vec3(1.0,0.2,0.1));
}
Console::~Console()
{
    delete error;
}
void Console::tick()
{
    float pad=2.0,alpha=0.8;float height=80.0;
    lines=Script::errors.size();
    if(lines>0){
        float x=-Shader::getResolution().x/2.0;
        float y=-Shader::getResolution().y/2.0;
        sbar.set(emenu->inspector->x-2*pad,y+height-4,height*2.0,height-8);
        error->width=emenu->inspector->x-x-50.0;
        sp->render(x,y,emenu->inspector->x-x,height,vec3(0.05),alpha);
        sp->render(x+pad,y+pad,emenu->inspector->x-x-2*pad,height-2*pad,vec3(0.09,0.1,0.11),alpha);
        sbar.tick();
        for(int i=0;i<Script::errors.size();i++){
            error->setText(Script::errors[i]);
            error->setPos(x+pad*2,y+height-2*pad-(i+1)*error->lheight);
            error->tick();
        }
    }
    
}


 Ewindow::Ewindow(float x, float y, float w, float h):x(x),y(y),w(w),h(h)
{
    bar=new Button(x,y,w,20.0);
    bar->uColor=vec3(0.15,0.16,0.17);bar->aColor=vec3(0.09,0.10,0.11);
    close=new Button(x+w-20.0,y,20.0,20.0);
    close->uColor=vec3(0.8,0.2,0.2);close->aColor=vec3(0.8,0.2,0.2)/2;
}
Ewindow::~Ewindow()
{
    delete bar;delete close;
   for(int a=0;a<buttons.size();a++){
       delete buttons[a];
   }
   for(int a=0;a<menus.size();a++){
     delete  menus[a];
   }
}
void Ewindow::addButton(float tx, float ty, TextButton* button)
{
    db.push_back(vec2(tx,ty));
    buttons.push_back(button);
    sort();
}
void Ewindow::addMenu(float tx, float ty,float tw,float th, Menu* menu)
{
    dm.push_back(vec2(tx,ty));
    menu->width=tw;menu->height=th;
    menus.push_back(menu);
    sort();
}


void Ewindow::sort()
{
    bar->move(x,y);close->move(x+w-20.0,y);
   for(int a=0;a<buttons.size();a++){
       buttons[a]->setPos(x+db[a].x,y+db[a].y);
   }
   for(int a=0;a<menus.size();a++){
       menus[a]->x=x+dm[a].x;menus[a]->y=y+dm[a].y;
       menus[a]->sort();
   }
}
 
void Ewindow::tick()
{
    float pad=3;

    sp->render(x,y,w,-h,vec3(0.19,0.20,0.21));
    sp->render(x+pad,y-pad,w-pad*2,-h+pad*2,vec3(0.28,0.3,0.32));

    bar->tick();close->tick();
    if(close->justDown){EditorUI::awindow=0;}

    if(bar->dragged){
        vec2 mm=Input::getMousedelta();
        x+=mm.x;y-=mm.y;
        sort();
    }
    
   for(int a=0;a<buttons.size();a++){
       buttons[a]->tick();
   }
   for(int a=0;a<menus.size();a++){
       menus[a]->tick();
   }
}

Console* console;
EditorUI::EditorUI()
{
   emenu=new EMenu(0);
   console=new Console;
   sp=Sprite::sp;activeMenu=0;
   
   play=new TextButton(0,0,"Play",0);
   reload=new TextButton(0,0,"Reload",0);
   prefab=new TextButton(0,0,"prefabs",0);
   common=new TextButton(0,0,"common",0);
   prefab->isToggle=common->isToggle=true;
   prefab->activeColor=vec3(1.0,0.8,0.4);
   common->activeColor=vec3(0.99,0.6,0.2);
   
   actile=new Label("active tile",-160,0,vec3(1,1,0.4),16);
   play->isToggle=true;play->activeColor=vec3(1.0,0.6,0.2);
   prefab->bgColor=common->bgColor=reload->bgColor=vec3(0.3,0.4,0.5)*0.8;
   play->bgColor=prefab->bgColor*1.4;prefab->bgColor=prefab->bgColor*1.4;
   
   std::string bnames[]={"File","Edit","View","Window","Help"};
   std::string fmn[]={"New","Open","New Scene","Save","Export"},
   emn[]={"Undo","Redo","Copy","Paste","Preferences","Scenes"};
   float w=0,y=Shader::sc_res.y/2;
   for(int i=0;i<5;i++){
       mbutts[i]=new TextButton(-Shader::sc_res.x/2+w,y,bnames[i],0);
        w+=mbutts[i]->transform.scale.x;
        mbutts[i]->bgColor=vec3(0.27,0.3,0.33);
    }
    
   file=mbutts[0];edit=mbutts[1];view=mbutts[2];window=mbutts[3];help=mbutts[4];
   edmenu=new Menu(edit->transform.position.x,edit->transform.position.y,0);
   fimenu=new Menu(file->transform.position.x,file->transform.position.y,0);
   edmenu->scrollable=false;fimenu->scrollable=false;
   EditorUI::resize();
   for(int i=0;i<6;i++){
        TextButton* tb= edmenu->addButton(emn[i]);
        tb->bgColor=vec3(0.32,0.6,0.6);
        tb->label->color=vec3(2.0);
   }
   slb=edmenu->tbuttons[5];
    for(int i=0;i<5;i++){
        TextButton* tb= fimenu->addButton(fmn[i]);
        tb->bgColor=vec3(0.32,0.6,0.6);
        tb->label->color=vec3(2.0);
   }
   newproj=fimenu->tbuttons[0];open=fimenu->tbuttons[1];
   
   nscnwindow=new Ewindow(-150.0,150.0,250.0,100.0);
   nscnwindow->addButton(20.0,-50,new TextField(0,0,140.0,"new_scene",0));
   nscnwindow->addButton(180,-50,new TextButton(0,0,"Add",0));
//    nscnwindow->addButton(225,-80,new TextButton(0,0,"cancel",0));
   
   scnwindow=new Ewindow(-150.0,150.0,300.0,300.0);
   scnwindow->addMenu(10.0,-20.0,200.0,240.0,new EList<Scene>(0,0,0,Scene::ascenes));
   scnwindow->addButton(245,-50,new TextButton(0,0,"Add",0));
   scnwindow->addButton(225,-80,new TextButton(0,0,"Remove",0));
   Menu* tm=new Menu(0,0,0);tm->visible=false;
   for(int a=0;a<Scene::scenes.size();a++){tm->addButton(Scene::scenes[a]->name);}
   scnwindow->addMenu(245,-50.0,200.0,240.0,tm);
}
void EditorUI::resize(){
  emenu->x=-Shader::getResolution().x/2;emenu->y=Shader::getResolution().y/2;
  emenu->inspector->x=Shader::getResolution().x/2-emenu->inspector->w;
  emenu->inspector->y=Shader::getResolution().y/2;
  emenu->inspector->h=-emenu->inspector->y*2.0;
  tbh=emenu->inspector->scomp->sceneSelect->title->transform.scale.y;
  emenu->y-=tbh-1.0;
  emenu->sort();
  emenu->inspector->y-=tbh;
  emenu->inspector->h+=tbh;
  if(emenu->pactiveScene==Scene::activeScene)
  emenu->inspector->sort();
  emenu->inspector->scomp->sceneSelect->width=120;
  emenu->inspector->scomp->sceneSelect->set(-16.0-60,Shader::getResolution().y/2.0-tbh);
  emenu->inspector->scomp->sceneSelect->title->setPos(-15.0-60,Shader::getResolution().y/2.0-tbh);
  float w=-Shader::sc_res.x/2,y=Shader::sc_res.y/2-tbh;
  for(int i=0;i<5;i++){
       mbutts[i]->setPos(w,y);  
       w+=mbutts[i]->transform.scale.x;
  }
  play->setPos(-115-60,y);reload->setPos(-75-60,y);
  common->setPos(225,y);prefab->setPos(155,y);
  actile->setPos(-360,y-20);
  
  fimenu->set(mbutts[0]->transform.position.x,mbutts[0]->transform.position.y);
  edmenu->set(mbutts[1]->transform.position.x,mbutts[1]->transform.position.y);
}


void EditorUI::update(){
     emenu->tick();
     console->tick();
     sp->render(-Shader::sc_res.x/2,Shader::sc_res.y/2,Shader::sc_res.x,-tbh,vec3(0.08,0.1,0.12),0.9);
     play->toggle=Scene::play;
     play->tick();reload->tick();
     common->tick();prefab->tick();
     Inspector::scomp->tick();
     //tick menu buttons
     for(int i=0;i<5;i++){mbutts[i]->tick();}
     // tick menus
     
     
     if(activeMenu){
         activeMenu->tick();
         for(int k=0;k<activeMenu->tbuttons.size();k++){
                if(activeMenu->tbuttons[k]->down&&!activeMenu->tbuttons[k]->justDown)
                {activeMenu=0;break;}
         }
    }
     // Menus and their logic
     if((activeMenu==fimenu&&fimenu->tbuttons[2]->justDown)||Input::getKeyDown(KEY_LCTRL,KEY_N)){
        // create new scene
        awindow=nscnwindow;
        Input::typing=true;
        ((TextField*)nscnwindow->buttons[0])->typing=true;
     }
     bool dwn=activeMenu&&(!activeMenu->hover()&&Input::getMouseButtonDown(0));
     if(activeMenu&& (Input::getKeyDown(KEY_ESCAPE)||dwn)){activeMenu=0;}
     
     if(edit->justDown){
         activeMenu=edmenu;
      }
      else if(file->justDown){
          activeMenu=fimenu;
    }
    //
        if(slb->justDown){slb->justDown=false;awindow=scnwindow;}
    //
     if(awindow){awindow->tick();}
     if(awindow==scnwindow){
         // scene window ==> menu[0] = ordered list of scenes. menu[1] = list of all scenes to choose
         bool b=0;
         for(int a=0;a<awindow->menus[1]->tbuttons.size();a++){if(awindow->menus[1]->tbuttons[a]->justDown){b=1;break;}}
         if(b && awindow->menus[1]->visible)
         {
             Scene* sc=Scene::findScene(awindow->menus[1]->active->label->text);
             if(sc){Scene::ascenes.push_back(sc);((EList<Scene>*)scnwindow->menus[0])->addButton(sc);}
             awindow->menus[1]->visible=0;
             
        }
         if(awindow->buttons[0]->justDown)awindow->menus[1]->visible=true;
         // removal of ascene 
         if(awindow->buttons[1]->justDown){
             TextButton* abutt=scnwindow->menus[0]->active;
             int n=-1;
             if(abutt){
                 Scene* ass=((ObjButton<Scene>*)abutt)->object;
                 for(int p=0;p<Scene::ascenes.size();p++){
                     if(Scene::ascenes[p]==ass){n=p;break;}
                 }
                 if(n>-1){Scene::ascenes.erase(Scene::ascenes.begin()+n);}
                 scnwindow->menus[0]->active=0;
             }
         }
         // end of remove
    }
    else if(awindow==nscnwindow){
        if(nscnwindow->buttons[1]->justDown||Input::getKeyDown(KEY_ENTER)||Input::getKeyDown(KEY_KP_ENTER)){
            printf(" new scene created!!! \n ");
            Scene* scn=new Scene(nscnwindow->buttons[0]->label->text);
            scn->setActive();
            Scene::scenes.push_back(scn);
            Scene::ascenes.push_back(scn);
            Inspector::scomp->sceneSelect->addButton(scn);
            Inspector::scomp->sceneSelect->setActive(scn->name);
            EMenu::emenu->selected.clear();
            EMenu::emenu->reset();
            Inspector::refresh();
            awindow=0;
        }
    }
    /////////////
    
}
    
    
EditorUI::~EditorUI()
{
    delete EMenu::emenu;delete edmenu;delete fimenu;
    delete scnwindow;delete nscnwindow;delete actile;
    delete play;delete reload;delete common;delete prefab;
    for(int i=0;i<5;i++)
        delete mbutts[i];
    
}
