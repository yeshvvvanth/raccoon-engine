#include"textfield.h"
#include"../render/script.h"
Ibeam* TextField::ibeam=0;
bool show=true;
char fbuff[128]; 
TextField::TextField(float x, float y,float width, std::string ltex,Font* fnt): TextButton(x, y,ltex,0),typing(false)
{
  float h=label->lheight;
  type=3;
  resize(x,y,width,h+2*padY); 
  bgColor=vec3(0.19,0.20,0.21);
}

FtextField::FtextField(float x, float y, float width,float val, std::string ltex, Font* fnt):
TextField(x, y, width,"0", fnt),value(val),stop(false)
{
  type=4;
  clamp=0;
  tb=new TextButton(x,y,ltex,fnt);
  tb->bgColor=vec3(0.48,0.5,0.52)*0.8;tb->activeColor=tb->bgColor*1.9;
  TextButton::setPos(x+tb->transform.scale.x,y);
  snprintf(fbuff, sizeof(fbuff), "%.2f",val);
  label->setText(fbuff);
}
void FtextField::setPos(float X, float Y)
{
  tb->setPos(X,Y);
  TextButton::setPos(X+tb->transform.scale.x, Y);
}
#define max(a,b) a<b?b:a
#define norm(a) a<0?0:(a>1?1:a)

void FtextField::draw()
{
  tb->tick();
  if(typing){Input::num=true;}
  

  if(tb->dragged){
    float vdx=Input::getMousedelta().x*2.0-Input::getMousedelta().y*4.0;
    if(Input::getKey(KEY_LCTRL))vdx=0;
    float v=float(vdx)/100;
    if(value!=v)
    {
    if(clamp==1){value+=v;value=max(value,0);}
    else if(clamp==2){value+=v/200;value=norm(value);}
    else if(clamp==4){value+=v;value=(max(value,0));}
    else{value+=v;}
    
    snprintf(fbuff, sizeof(fbuff), "%.2f",value);
    label->setText(fbuff);
    }
  }
  if( stop )
  {
//     printf("pikaa");
    char a=label->text[0];
      if(1/*||(a>47 && a<58)|| a==45 || label->text=="" */)
      {
        if(label->text==""){label->text="0.0";}
//         printf("chu\n");
    //  	  value=std::stof(label->text);
        lua_State*L= Script::L;

        std::string texx="fu45="+label->text;
        luaL_dostring(L,texx.c_str());
        lua_getglobal(L,"fu45");
        if(lua_isnumber(L,-1)){value=lua_tonumber(L,-1);}
        lua_pop(L,1);
        snprintf(fbuff, sizeof(fbuff), "%.2f",value);
        label->setText(fbuff);
      }
    Input::num=false;
  }
  
  stop=typing;
  TextField::draw();
  stop= stop && !typing;
///////////////////////
}
void FtextField::setValue(float v)
{
    value=v;
    snprintf(fbuff, sizeof(fbuff), "%.2f",v);
    label->setText(fbuff);
}


FtextField::~FtextField()
{
  delete tb;
}



void TextField::check()
{
    if(!Input::typing){typing=false;}
    if(typing && !down && Input::getMouseButton(0)){typing=false;}
    
    if( justDown )
    {
      typing=!typing;
      Input::typing=typing;
      Input::text=label->text;
      Input::textCurPos=label->text.size();
    }
    if(typing &&(label->text!=Input::text) ){label->setText(Input::text);show=true;}
}

void TextField::draw()
{
  check();
  
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  Font::shader->use();
  
  glBindVertexArray(vao);
  glBindTexture(GL_TEXTURE_2D,Shader::twhite);
  glUniform1f(glGetUniformLocation(label->font->shader->PID,"fnt"),1.0f);
  glUniform1f(Font::shader->uniAlpha,1.0f);
  
  glUniform3f(uniTint, bgColor.x     ,bgColor.y     ,bgColor.z     );
  if(typing){glUniform3f(uniTint, 0.1,0.1,0.1);}
  
  transform.update();
  glUniformMatrix4fv(shader->uniModel, 1, GL_FALSE,transform.model.elements);
  glDrawArrays(GL_TRIANGLES,0,6);
  glUniform1f(glGetUniformLocation(label->font->shader->PID,"fnt"),0.0f);
  glBindVertexArray(0);
  
  if(typing)ibeam->draw(label->x+label->xoffsets[Input::textCurPos],label->y-5);
  label->tick();
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

Ibeam::Ibeam()
{
  GLuint vbo=0;
  glGenVertexArrays(1,&vao);
  glGenBuffers(1,&vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER,vbo);
  
  float verts[4]={0.0f,0.0f,0.0f,1.0f};
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) *4, verts, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0,0);
  
  height=25;
  model=mat4::Scale(vec3(1.0,height,1.0));
  color=vec3(1.0f,1.5f,0);
  glLineWidth(4.5);
}
double tim=0.0;
void Ibeam::draw(float X,float Y)
{
  model[12]=X;
  model[13]=Y;
   Font::shader->use();
   glBindTexture(GL_TEXTURE_2D,Shader::twhite);
   glBindVertexArray(vao);
   glUniform3f(Label::uniTint,color.x,color.y,color.z);
   glUniformMatrix4fv(Font::shader->uniModel, 1, GL_FALSE,model.elements);
   
   if(glfwGetTime()-tim<0.6f){glDrawArrays(GL_LINES,0,2);}
   else if(glfwGetTime()-tim<1.2f){}
   else{tim=glfwGetTime();}
   
   if((Input::getKeyDown(KEY_RIGHT)||Input::arrows[1]) && Input::textCurPos<Input::text.length())
      {Input::textCurPos++;tim=glfwGetTime();}
   else if((Input::getKeyDown(KEY_LEFT)||Input::arrows[0]) && Input::textCurPos>0)
      {Input::textCurPos--;tim=glfwGetTime();}
   else if(show){tim=glfwGetTime();}
   show=false;
}



