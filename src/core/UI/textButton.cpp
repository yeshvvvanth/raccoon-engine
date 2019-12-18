#include"textButton.h"

TextButton::TextButton(float x, float y, std::string ltex,Font* fnt,bool islabel): Button(x, y,30,20)
,bgColor(vec3(0.19,0.2,0.21)),activeColor(vec3(0.3,0.6,0.8)),visible(true)
{
  float scale=13.0f;
  Font* font=fnt!=0?fnt:Font::efonts[0];
  Dim=0.4f;
  padX=6.0f;
  padY=6.0f;
  isLabel=islabel;
  type=1;
  label=new Label(ltex,x+padX,y+padY,vec3(1.0f,1.0f,1.0f));
  if(!isLabel)
  resize(x,y,label->lwidth+2*padX,label->lheight+2*padY);
  else type=5;
}

void TextButton::setSize(float W, float H)
{
  float px=transform.position.x;
  float py=transform.position.y;
  
  label->multiline=isLabel;
  label->width=W;
  label->height=H;
  if(W==0){resize(px,py,W,transform.scale.y);}
  else if(H==0){resize(px,py,transform.scale.x,H);}
  else{resize(px,py,W,H);}
  label->setPos(transform.position.x+padX,transform.position.y+padY);
}

void TextButton::setPos(float X, float Y, float W, float H)
{
    label->multiline=isLabel;
   label->width=W;label->height=H;
   resize(X,Y,W,H);
   label->setPos(transform.position.x+padX,transform.position.y+padY);
}
void TextButton::setPos(float X, float Y)
{
    label->multiline=isLabel;
    label->width=transform.scale.x;
  label->height=transform.scale.y;
   float h=label->font->lheight*label->scale;
   resize(X,Y,this->transform.scale.x,this->transform.scale.y);
   label->setPos(transform.position.x+padX,transform.position.y+padY);
}


TextButton::~TextButton()
{
  delete label;
}


void TextButton::draw()
{
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
//   glEnable(GL_BLEND);
  Shader* tshader=0;
  if(!isLabel){
    glBindVertexArray(vao);
    vec3 bgcolor=bgColor;
    if(texture){
        tshader=Sprite::shader;
        texture->bind();
        bgcolor=vec3(1.0);
//         printf("tbutton texrure = %s \n ",texture->name.c_str());
    }
    else{
        tshader=Font::shader;
        glBindTexture(GL_TEXTURE_2D,Shader::twhite);
        tshader->use();
        glUniform1f(Font::uniFnt,1.0f);
    }
    tshader->use();
//     glUniform1f(tshader->uniAlpha,alpha);
    
    if(down||dragged ||(isToggle && toggle))      {glUniform3f(uniTint, activeColor.x ,activeColor.y ,activeColor.z );}
    else if(over) {glUniform3f(uniTint, bgcolor.x*Dim ,bgcolor.y*Dim ,bgcolor.z*Dim );}
    else          {glUniform3f(uniTint, bgcolor.x     ,bgcolor.y     ,bgcolor.z     );}
    
    transform.update();
    glUniformMatrix4fv(tshader->uniModel, 1, GL_FALSE,transform.model.elements);
    glDrawArrays(GL_TRIANGLES,0,6);
    glUniform1f(Font::uniFnt,0.0f);
    glBindVertexArray(0);
  }
  else{
      
  }
  label->tick();
//   glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  
}




