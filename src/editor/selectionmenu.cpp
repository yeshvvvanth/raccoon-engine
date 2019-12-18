#include"entitymenu.h"


///  CONTEXT MENU  /////////////////////////
void ContextMenu::tick()
{
  for(int a=0;a<tbuttons.size();a++)
  {tbuttons[a]->down=tbuttons[a]->pdown=tbuttons[a]->justDown=false;}
 
  if(visible){
    for(int a=0;a<tbuttons.size();a++){tbuttons[a]->tick();}
    vec2 mp=Shader::scrMousePos();
    if(Input::getMouseButtonDown(1))set(mp.x,mp.y);
    
    float w,h,x,y;
    w=tbuttons[0]->transform.scale.x;
    h=(tbuttons.size()*tbuttons[0]->transform.scale.y);
    x=tbuttons[0]->transform.position.x;
    y=tbuttons[0]->transform.position.y+tbuttons[0]->transform.scale.y;
    
    
    if(!(mp.x>x && mp.x<x+w && mp.y<y && mp.y>y-h)&&Input::getMouseButton(0))
	{visible=false;}
    for(int a=0;a<tbuttons.size();a++){if(tbuttons[a]->down)visible=false;}
  }
}

ContextMenu::ContextMenu(float X, float Y, Font* fnt): Menu(X, Y, fnt)
{
  visible=(false);
   scrollable=false;
}

///  SELECTION MENU  /////////////////////////
template<class T>
SelectionMenu<T>::SelectionMenu(float X, float Y, Font* fnt,std::vector<T*> vec,bool highlact): Menu(X, Y, fnt),activeObj(0)
{
  title=new TextButton(x,y,"select",fnt);
  highlightactive=highlact;
  if(!highlightactive){
    none=addButton(0);
    active=none;
    title->label->setText(none->label->text);
    none->label->color=vec3(0.8,0.4,0.3);
  }
  active=0;
  height=240;
  for(int g=0;g<vec.size();g++){addButton(vec[g]);}
  visible=false;
  title->bgColor=vec3(0.46,0.5,0.54)*0.7;
/*
  if(width<title->transform.scale.x){
      width=title->transform.scale.x;
      sort();
  }
  else  if(width>title->transform.scale.x){
      title->transform.scale.x=width;
  }
  */
}
template <class T>
void SelectionMenu<T>::setActive(std::string name)
{
  for(int i=0;i<tbuttons.size();i++)
    { 
      TextButton *tb= tbuttons[i];
      if(tb->label->text==name){active=tb;break;}
    }
  if(active){
      title->label->setText(active->label->text);
      activeObj= ((ObjButton<T>*) active)->object;
  }
}



template<class T>
ObjButton<T>* SelectionMenu<T>::addButton(T* obj)
{
     width=200;
  if(tbuttons.size()&&tbuttons[tbuttons.size()-1]->transform.scale.x>title->transform.scale.x)
  {
        title->setSize(tbuttons[tbuttons.size()-1]->transform.scale.x,title->transform.scale.y);
  }
    //////////////////////////
    ObjButton<T>* but;
  if(obj) but =new ObjButton<T>(x,y,obj->name,obj,0);
  else    but=new ObjButton<T>(x,y,"none",0,0);
  but->isToggle=highlightactive;
  float& butwidth=but->transform.scale.x,&butheight=but->transform.scale.y;
  if(highlightactive&& tbuttons.size()==1){active=but;activeObj=obj;title->label->setText(obj->name);}
  
  width=butwidth>width?butwidth:width;
  ih=butheight>ih?butheight:ih;
  
  but->setPos(x,y-(tbuttons.size()+1)*butheight,width,butheight);
  tbuttons.push_back(but);
  
  Menu::sort();
  return but;
}

template<class T>
void SelectionMenu<T>::tick()
{
  if( Input::getKeyDown(KEY_ESCAPE)||(!hover()&&Input::getMouseButtonDown(0)) )
      {visible=false;}
  title->tick();
  if(title->justDown){visible=true;}
  if(visible){

    Menu::tick();
    
	if(active&&   ((ObjButton<T>*)active)->object!=activeObj   ){
 	    activeObj=((ObjButton<T>*)active)->object;
	    title->label->setText(active->label->text);
	    visible=false;
	}

  }
  ///////////
  float w,h,x,y;
  w=title->transform.scale.x;
  h=(title->transform.scale.y+tbuttons.size()*tbuttons[0]->transform.scale.y);
  x=title->transform.position.x;
  y=title->transform.position.y+title->transform.scale.y;

  vec2 mp=Shader::scrMousePos();
  
}
template<class T>
SelectionMenu<T>::~SelectionMenu()
{
  delete title;
}
template struct SelectionMenu<Script>;
template struct SelectionMenu<Entity>;
template struct SelectionMenu<RawMesh>;
template struct SelectionMenu<Animation>;
template struct SelectionMenu<Texture>;
template struct SelectionMenu<Shader>;
template struct SelectionMenu<Scene>;
template struct SelectionMenu<Audio>;
template struct SelectionMenu<Font>;
//////////////////////////
