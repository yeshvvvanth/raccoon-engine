#include"entitymenu.h"
Sprite* Inspector::sp=0;
AddComp* Inspector::addcomp=0;
SceneComp* Inspector::scomp=0;
bool Inspector::scroll,Inspector::scrolling;
short Inspector::h=0,Inspector::w=0;
short Inspector::dy=0,Inspector::dx=0;
ContextMenu *Inspector::cmenu=0;
TextButton *newComp,*delComp,*expand; 
ScrollBar Inspector::sbar;
void scrollbar();
Inspector::~Inspector()
{
  printf("deleting shite!!!!!!!!!!!!!\n\n");
   for(int i=0;i<Components.size();i++){delete Components[i];}
   Components.clear();
   delete addcomp;
   delete scomp;
   delete cmenu;
}
Inspector::Inspector()
{
  Shader::getResolution();
  w=80*3;h=-Shader::sc_res.y;
  x= Shader::sc_res.x/2-w;
  y= Shader::sc_res.y/2;
   
  cmenu=new ContextMenu(0.0,0.0,0);
  newComp=cmenu->addButton("Add Empty");
  delComp=cmenu->addButton("Delete Component");
  sp=Sprite::sp;

  addcomp    =new AddComp;
  addcomp->mc=new MeshComp;
  addcomp->lc=new LightComp;
  addcomp->ac=new SoundComp;
  addcomp->cc=new camComp;
  addcomp->uc=new UIComp;
  addcomp->pc=new PhysicsComp;
  addcomp->sc=new ScriptComp;
  
  Components.push_back(new EntComp);
  Components.push_back(new Transpector);
  Components.push_back(new WPcomp);
   Components.push_back(new GMcomp);
  Components.push_back(addcomp->mc);
  Components.push_back(addcomp->lc);
  Components.push_back(addcomp->ac);
  Components.push_back(addcomp->cc);
  scomp=new SceneComp;
  Components.push_back(addcomp->uc);
  Components.push_back(addcomp->pc);
  Components.push_back(addcomp->sc);
 
  sort();
  Inspector::refresh();
}

void Inspector::tick()
{
  sp->render(x,y,w,h,vec3(0.09,0.1,0.11),0.82);
  // cmenu  
  vec2 mp=Shader::scrMousePos();
  bool mip=mp.x>x&& mp.x<x+w && mp.y<y && mp.y>y+h;
  if(Input::getMouseButtonDown(1)&&mip){cmenu->visible=true;}
  if(!EMenu::activeEntity||EMenu::activeEntity->source.length()){
    Components[1]->tick();
    return;
  }
  // update from bottom up so that active menus don't get overlapped by lower ones 
  addcomp->tick();
  for(int i=Components.size()-1;i>=0;i--)
  {
      bool mip2=mip&&mp.y<Components[i]->y && mp.y> Components[i]->y+Components[i]->H;
      if(!Components[i]->empty && mip2 && delComp->justDown ){
          printf("delete Component type=%d \n",Components[i]->type);
          Components[i]->remove();
      }
      if(!Components[i]->empty && mip2 && Input::getMouseButtonDown(2) ){
          Components[i]->expand=!Components[i]->expand;
          refresh();
      }
      if(!Components[i]->empty){
          sp->render(x,Components[i]->y+Components[i]->H+32.0,w,2.0f,vec3(0.5),1.0);
          Components[i]->tick();
      }
  } 
  cmenu->tick();
  scrollbar();
}

void Inspector::refresh()
{
     if(EMenu::activeEntity&&!EMenu::activeEntity->source.length()){
        printf("inspector REFRESH\n");
        H=0;dy=0;
        if(scroll)y+=dy;
        for(Component* ie:Components)
        {
            ie->get();
            if(!ie->empty)ie->sort();
        }
                
        addcomp->get();addcomp->sort();
        if(scroll)y-=dy;
     }
     else{
         H=-45;dy=0;
         Components[1]->get();
         Components[1]->sort();
    }
}

Component::Component():H(0),y(0),empty(false),expand(false){}
Component::~Component(){printf("&&&&&&&&&&&&&&&&& deleting Component \n");}
void Component::remove(){}
void Inspector::sort()
{
    if(EMenu::activeEntity&&!EMenu::activeEntity->source.length()){
        printf("inspector sort\n");
        //    x=Shader::sc_res.x/2-w;y=-Shader::sc_res.y/2;
        //   sp->resize(x,y,-77*3,-y*2);
        sp->resize(x,y,w,h);
        if(scroll)y+=dy;
        H=0;
        for(int i=0;i<Components.size();i++){if(!Components[i]->empty)Components[i]->sort();}
        addcomp->sort();
        if(scroll)y-=dy;
        
        short ih=Inspector::H-180;
        sbar.set(x+w,y,-ih,-h);
    }
    else{
        H=-45;dy=0;
        Components[1]->sort();
    }
}

void scrollbar()
{
    short ih=Inspector::H-180,sbw=-12.0,pad=0;
    float x=Inspector::x,y=Inspector::y,
    height=Inspector::h,width=Inspector::w;short &dy=Inspector::dy;
    bool mv=(!Inspector::addcomp->mc->meshSelect->selMenu->visible);
    bool tv=(!Inspector::addcomp->mc->textureSelect->selMenu->visible);
    Inspector::scroll=(ih<height);bool &drag=Inspector::scrolling;
    Sprite* sp=Sprite::sp;
    vec2 mp=Shader::scrMousePos();
    /// SCROLL BAR IMPLEMENTATION
    if(Inspector::scroll){
        ScrollBar& sbar=Inspector::sbar;
        sbar.tick();
        // mouse scroll checking
        //don't mouse wheel scroll when addcomp menu is active
        bool hover=!Inspector::addcomp->cmenu->visible&&Inspector::addcomp->active==0;
        hover=hover&&(mp.x<x+width+pad &&mp.x> x&& mp.y<y&&mp.y>y+height)&&Input::scroll&&mv&&tv;
        
        if(hover){
            dy+=(Input::getMousedelta().y-Input::scroll*30.0)*(ih/height);
            if(dy<0)dy=0;else if(dy>-ih+height)dy=-ih+height;
            Inspector::sbar.dy=dy;
        }
        if(hover || (sbar.pdy!=sbar.dy)){Inspector::sort();}
        dy=sbar.dy;
         
    }
}
///
