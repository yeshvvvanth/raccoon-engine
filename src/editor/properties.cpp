#include"properties.h"
#include"entitymenu.h"
char flbuff[128];
///         PROP_VECTOR        /////////////////////////////
Property::~Property()
{
}
Property::Property(short h,char type):h(h),type(type)
{
  
}


vec3 Prop_Vector::toVec(){
  return vec3(buttons[0]->value,buttons[1]->value,buttons[2]->value);
}
void Prop_Vector::fromVec(vec3 v){
  buttons[0]->value=v.x;buttons[1]->value=v.y;buttons[2]->value=v.z;
  for(int i=0;i<3;i++){
      snprintf(flbuff, sizeof(flbuff), "%.2f",buttons[i]->value);
      buttons[i]->label->setText(flbuff);
  }
}

Prop_Vector::Prop_Vector(std::string name,vec3 vec):Property(40,0){
  label= new Label(name,0,0,vec3(0.99,0.8,0.6)*0.8);
  
  short w=48,bh=20;
  buttons[0]=new FtextField(0,0,w,0,"X",0);
  buttons[1]=new FtextField(0,0,w,0,"Y",0);
  buttons[2]=new FtextField(0,0,w,0,"Z",0);
  fromVec(vec);
}
void Prop_Vector::sort(short &tH){
   short H=Inspector::H,x=Inspector::x,
   y=Inspector::y,w1=x+5,w=buttons[0]->transform.scale.x+25;

   label->setPos(x+5,y+H+tH+25);
   for(int i=0;i<3;i++)
    {buttons[i]->setPos(w1+w*i,y+H+tH);}
   tH-=h;
}
void Prop_Vector::tick(){
  
    label->tick();
  for(int i=0;i<3;i++){
    buttons[i]->tick();
  }
}
Prop_Vector::~Prop_Vector(){delete label;delete buttons[0],delete buttons[1];delete buttons[2];}

vec2 Prop_Vec2::toVec(){
  return vec2(buttons[0]->value,buttons[1]->value);
}
void Prop_Vec2::fromVec(vec2 v){
  buttons[0]->value=v.x;buttons[1]->value=v.y;;
  for(int i=0;i<2;i++){
      snprintf(flbuff, sizeof(flbuff), "%.2f",buttons[i]->value);
      buttons[i]->label->setText(flbuff);
  }
}

Prop_Vec2::Prop_Vec2(std::string name,vec2 vec):Property(40,0){
  label= new Label(name,0,0,vec3(1.0f,0.7,0.7));
  
  short w=50;
  buttons[0]=new FtextField(0,0,w,0,"X",0);
  buttons[1]=new FtextField(0,0,w,0,"Y",0);
}
void Prop_Vec2::sort(short &tH){
   short H=Inspector::H,x=Inspector::x,
   y=Inspector::y,w1=x+25,w=buttons[0]->transform.scale.x+35;

   label->setPos(x+5,y+H+tH+25);
   for(int i=0;i<2;i++)
	{buttons[i]->setPos(w1+w*i,y+H+tH);}
   tH-=h;
}
void Prop_Vec2::tick(){
  
    label->tick();
  for(int i=0;i<2;i++){
    buttons[i]->tick();
  }
}
Prop_Vec2::~Prop_Vec2(){delete buttons[0],delete buttons[1];delete label;}

///         PROP_SELECT        /////////////////////////////
template<class T>
Prop_Select<T>::Prop_Select(std::string name,std::vector< T* > vec,bool nonone): Property(40,0)
{
  selMenu=new SelectionMenu<T>(0,0,0,vec,nonone);
  label=new Label(name,0,0,vec3(1.0));

}
template<class T>
void Prop_Select<T>::sort(short int& tH)
{
   short H=Inspector::H,x=Inspector::x,
	 y=Inspector::y;
  //reset positions
  short w=label->lwidth+40;//Max(label->lwidth+10,90);
  label->setPos(x+5,y+H+tH+27);
  selMenu->set(x+5,y+H+tH);
  selMenu->title->setPos(x+5,y+H+tH);
   tH-=h;
}
template<class T>
void Prop_Select<T>::tick()
{
  label->tick();
  selMenu->tick();
}
template<class T>
Prop_Select<T>::~Prop_Select()
{
  delete selMenu;delete label;
}
template struct Prop_Select<Script>;
template struct Prop_Select<Entity>;
template struct Prop_Select<RawMesh>;
template struct Prop_Select<Animation>;
template struct Prop_Select<Texture>;
template struct Prop_Select<Shader>;
template struct Prop_Select<Audio>;
template struct Prop_Select<Font>;
///         PROP_STRING       /////////////////////////////

Prop_String::Prop_String(std::string name, std::string str): Property(40,0)
{
  label=new  Label(name,0,0,vec3(1.0));
  field=new TextField(0,0,80,str,0);
}

void Prop_String::sort(short int& tH)
{
   short H=Inspector::H,x=Inspector::x,
	 y=Inspector::y;
  //reset positions
  short w=label->lwidth+10;
  label->setPos(x+5,y+H+tH+5);
  field->setPos(x+5+w,y+H+tH);
  
  tH-=h;
}

void Prop_String::tick()
{
  label->tick();
  field->tick();
}

Prop_String::~Prop_String()
{
  delete label;delete field;
}
///         PROP_FLOAT      /////////////////////////////

Prop_Float::Prop_Float(std::string name, bool b): Property(40, 0)
{
  ftext=new FtextField(0,0,45,0.0,"<>",0);
  label=new Label(name,0,0,vec3(1.0));
}

void Prop_Float::sort(short int& tH)
{
   short H=Inspector::H,x=Inspector::x,
	 y=Inspector::y;
  //reset positions
  short w=label->lwidth+10;
  label->setPos(x+5,y+H+tH+5+5);
  ftext->setPos(x+5+w,y+H+tH+5);
  
  tH-=h;
}

void Prop_Float::tick()
{
  ftext->tick();
  label->tick();
}

Prop_Float::~Prop_Float()
{
  delete ftext;delete label;
}


///         PROP_BOOL      /////////////////////////////

Prop_Bool::Prop_Bool(std::string name, bool b): Property(40.0,0 )
{
  label=new Label(name,0,0,vec3(1.0));
  button=new Button(0.0f,0.0f,20.0f,20.0f);
  button->aColor=vec3(0.6,0.8,0.4)*1.2;
  button->uColor=vec3(0.6);
  button->isToggle=true;
}

void Prop_Bool::sort(short int& tH)
{
  short H=Inspector::H,x=Inspector::x,
  y=Inspector::y;
  //reset positions
  short w=label->lwidth+20;
  label->setPos(x+5,y+H+tH+5+5);
  button->resize(x+5+w,y+H+tH+6.0,20.0,20.0);
  tH-=h;
}

void Prop_Bool::tick()
{
  label->tick();
  vec3& p=button->transform.position,&s=button->transform.scale;
//   Sprite::render(p.x-4.0,p.y-4.0,s.x*1.4,s.y*1.4,vec3(0.4,0.6,0.2));
  Sprite::render(p.x-3.0,p.y-3.0,s.x*1.3,s.y*1.3,vec3(0.4));
  button->tick();
}

Prop_Bool::~Prop_Bool()
{
  delete button;delete label;
}


Prop_Slide::Prop_Slide(std::string name, bool b): Property(40.0,0 )
{
    label=new Label(name,0,0,vec3(1.0));
  slide=new Slider(0,0,50,30);
}
void Prop_Slide::sort(short& tH)
{
    short H=Inspector::H,x=Inspector::x,
	 y=Inspector::y;
  //reset positions
  short w=label->lwidth+20;
  label->setPos(x+5,y+H+tH+5+5);
  slide->setPos(x+15,y+H+tH+5+5);
  tH-=h;
}

void Prop_Slide::tick()
{
    label->tick();
    slide->tick();
}

Prop_Slide::~Prop_Slide()
{
    delete slide;delete label;
}

