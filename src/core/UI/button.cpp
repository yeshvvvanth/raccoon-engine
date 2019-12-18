#include"button.h"

#define INSIDE 
Button* Button::b1=0;
Button* Button::b2=0;
GLuint Button::uniTint=GL_INVALID_VALUE;

Button::Button(float x,float y,float width,float height):Sprite( x, y, width, height),
down(0),over(0),toggle(0),justDown(0),dragged(0),isToggle(0)
{
  pdown=over=down=0;type=0;
  uniTint= glGetUniformLocation(shader->PID,"tint");
  glUseProgram(shader->PID);
  glUniform3f(uniTint,1.0,1.0,1.0);
  
  uColor=vec3(1.0);
  aColor=vec3(0.8,0.6,0.3);
}
void Button::tick(bool scale){ 
  if(type!=6){
  pdown=down;
  glUseProgram(shader->PID);
  vec2 mp=shader->scrMousePos(scale);
  
  float &x=transform.position.x,&y=transform.position.y;
  float &width=transform.scale.x,&height=transform.scale.y;
    
  over= mp.x>x  &&  mp.x <x+width && 
        mp.y >y &&  mp.y <y+height;
  if(over){b1=this;over=(b2==this);}
  down=over && (Input::getMouseButtonDown(0)|| (pdown && Input::getMouseButton(0) ));
  
  if(down && !pdown){toggle=!toggle;justDown=true;}
  else{justDown=false;}
  
  if(!Input::getMouseButton(0)){dragged=false;}
  if(down){dragged=true;}

  draw();
  }
  else{
    color=uColor;
    Sprite::draw();
  }
}
void Button::draw()
{
  shader->use();
  float dim=0.9;
  if(down || (isToggle && toggle))      {color=aColor;}
  else if(over) {color=uColor*dim;}
  else          {color=uColor;}
  
  Sprite::draw();
  
}

void Button::setPos(float X, float Y)
{
    resize(X,Y,this->transform.scale.x,this->transform.scale.y);
}


Button::~Button()
{

}

Slider::Slider(float x,float y,float width,float height):Button( x, y, width, height),value(0.0)
{
    type=2;
    handle=new Button(x,y+height/2.0-10.0,20.0,20.0);
    uColor=vec3(0.8);
    handle->uColor=vec3(0.8,0.2,0.2);
//     texture=Texture::textures[0];
}

void Slider::tick(){
    
    Button::tick();
    handle->tick();
    if(handle->dragged){
        vec3 hp=handle->transform.position,hs=handle->transform.scale;
        vec3 op=transform.position,os=transform.scale;
        hp.x+=Input::getMousedelta().x;
        float dx=(os.x-hs.x);
        value=(hp.x- op.x )/dx;
        value= value>1?1:(value<0?0:value);
        handle->setPos(op.x+dx*value,op.y+os.y/2.0-hs.y/2.0);
    }
}

void Slider::setPos(float X, float Y)
{
    Button::setPos(X,Y);
    
    vec3 hp=handle->transform.position,hs=handle->transform.scale;
    vec3 op=transform.position,os=transform.scale;
    float dx=(os.x-hs.x);value=(hp.x- op.x )/dx;
    value= value>1?1:(value<0?0:value);
    handle->setPos(op.x+dx*value,op.y+os.y/2.0-hs.y/2.0);
    
}


Slider::~Slider()
{
    delete handle;
}


