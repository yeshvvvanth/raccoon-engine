#include"menu.h"

Menu::Menu(float X,float Y,Font* fnt):x(X),y(Y),width(0),height(0),visible(true),scroll(false)
{
  font= fnt!=0?fnt:Font::efonts[0];
  width=120;
  height=140;
  dx=(0);dy=(0);
  active=0;
  sbw=-11.0;
  islist=  false;
  heir=false;
  rep=false;
  scrollable=true;
  pad=4;
  ih=0;
//   sbar.set(x+width+4.0,y-4.0,ih,height);
}

Menu::~Menu()
{
    printf("$$$### deleting menu \n");
  for(int i=0;i<tbuttons.size();i++){delete tbuttons[i];}
}

TextButton* Menu::addButton( std::string text)
{
  TextButton* but=new TextButton(x,y,text,font);
  
  float& butwidth=but->transform.scale.x,&butheight=but->transform.scale.y;
  
  width=butwidth>width?butwidth:width;
  ih=butheight>ih?butheight:ih;
  
  but->setPos(x,y-(tbuttons.size()+1)*butheight,width,butheight);
  tbuttons.push_back(but);
  
  Menu::sort();
  return but;
}
void Menu::set(float X, float Y)
{
  x=X;y=Y;
  sort();
}

void Menu::sort()
{
    ih=0;
    for(int i=0;i<tbuttons.size();i++){
      TextButton* tb=tbuttons[i];
       float& tbwidth=tb->transform.scale.x,&tbheight=tb->transform.scale.y;
       float& tbx=tb->transform.position.x,&tby=tb->transform.position.y;
       ih+=tbheight;
       float twidth= scroll?width+sbw:width;
       width=(!scrollable&&width<tbwidth)?tbwidth:width;

       tb->setPos(x+pad,y-pad+dy-(i+1)*tbheight,twidth,tbheight);
    }
    sbar.set(x+width+4.0,y-4.0,ih,height);
}
bool Menu::hover(){
    vec2 mp=Shader::scrMousePos();
    float tw=width/*scroll?width+sbw:width*/;
    float th=scroll?height:ih;
    return (mp.x>x+pad&&mp.x<x+tw+pad&&mp.y<y-pad&&mp.y>y-th-pad);
}

ScrollBar::ScrollBar()
{
    x=0;y=0;sbw=10.0;height=40.0;scolour=vec3(0.9,0.4,0.3);
    ih=80.0;dy=0;drag=false;
}
void ScrollBar::set(float tx, float ty, float tih, float theight)
{
    x=tx;y=ty;ih=tih;height=theight;
//     dy=0;drag=pdrag=false;
//     sbw=pdy=0;
}
void ScrollBar::tick()
{
    float sby=y-((dy/ih)*height),sbh=-(height*height)/ih;
    vec2 mp=Shader::scrMousePos();vec3 scolor=scolour;
    bool hover=(mp.x<x &&mp.x> x-sbw && mp.y<sby&&mp.y>sby+sbh);
    pdrag=drag;pdy=dy;
    drag=hover&&Input::getMouseButton(0);
    drag=drag||(pdrag&&Input::getMouseButton(0));
    if(drag){
        scolor=scolor*0.7f;
        dy+=(Input::getMousedelta().y)*(ih/height);
        if(dy<0)dy=0;
        else if(dy>ih-height)dy=ih-height;
    }
    
    Sprite* sp=Sprite::sp;
    sp->render(x,y,-sbw,-height,vec3(0.8));
    sp->render(x,sby,-sbw,sbh,scolor);
}

void Menu::tick()
{
  if(visible)
  {
      Sprite *sp=Sprite::sp;
      scroll=scrollable&&(ih>height);
      vec2 mp=Shader::scrMousePos();
      //
        float pa=sp->alpha;
        
        float tw= scroll?width:width;
        float th= scroll?height:ih;
        float tp=pad*2;
        
        sp->render(x,y,tw+tp,-th-tp,vec3(0.14),1.0);
        sp->render(x+tp/4,y-tp/4,tw+tp/2,-th-tp/2,vec3(0.18,0.2,0.22),1.0);
     /*
       if(heir) 
       else
         sp->render(x,y,tw+tp,-th-tp,vec3(0.2),0.7);
       if(heir)
       else
         sp->render(x+tp/4,y-tp/4,tw+tp/2,-th-tp/2,vec3(0.6),1.0);
     */
        
    /// SCROLL BAR IMPLEMENTATION
    if(scroll){
        vec3 scolor=vec3(0.9,0.4,0.3);
        glClear( GL_STENCIL_BUFFER_BIT); 
        glEnable(GL_STENCIL_TEST);  
        glStencilFunc(GL_ALWAYS, 1, 1);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        sp->render(x+pad,y-pad,width+sbw,-height,vec3(1.0),0.0);
        glDisable(GL_STENCIL_TEST);  
        /// 
        sbar.tick();
        // mouse scroll checking
        bool hover=(mp.x<x+width+pad &&mp.x> x&& mp.y<y&&mp.y>y-height)&&Input::scroll;
        if(hover){
            dy+=(Input::getMousedelta().y-Input::scroll*4.0)*(ih/height);
            if(dy<0)dy=0;else if(dy>ih-height)dy=ih-height;
            sbar.dy=dy;
        }
        dy=sbar.dy;
        if(hover || (sbar.pdy!=sbar.dy)){sort();}
        
        glEnable(GL_STENCIL_TEST);  
        glStencilFunc(GL_EQUAL, 1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
    
    bool overMenu=(mp.x>x&&mp.x<x+width&&mp.y<y&&mp.y>y-height);
    for(int i=0;i<tbuttons.size();i++)
    {
        TextButton *tb=tbuttons[i];
        //it was a bad idea to not tick non-visible tbuttons
        //float ty=tb->transform.position.y,th=tb->transform.scale.y;
        //if(!scroll ||(ty <y-pad && ty+th > y-height-pad))
        {
            if(tb->visible)tb->tick();
            if(scroll && !overMenu){tb->down=tb->justDown=false;}
            if(tb->justDown){active=tb;}
            if(tb!=active){tb->toggle=false;}else{tb->toggle=true;}
        }
    }
    glDisable(GL_STENCIL_TEST);  
    /// REPOSITION OF TBUTTONS 
    bool b1= active && islist && active->dragged;
    bool release=rep && !b1;
    rep=b1;
    if(b1||release){
        int b=-1,a=-1;
        for(int p=0;p<tbuttons.size();p++){
            if(active==tbuttons[p])b=p;
        }
            TextButton *tb=0;
        for(int p=0;p<tbuttons.size();p++){
            if(tbuttons[p]->over && tbuttons[p]!=active){
                tb=tbuttons[p];a=p;break;
            }
        }
        if(tb ){
            vec3 p=tb->transform.position;
            float ph=tb->transform.scale.y;
            vec3 color=vec3(0.9,0.9,0.6);
            if(mp.y <p.y+(ph*0.25) && a>b){
                sp->render(p.x,p.y,tb->transform.scale.x,2.0,color);
                if(b>=0 && release)
                {
                    tbuttons.erase(tbuttons.begin()+b);
                    tbuttons.insert(tbuttons.begin()+a,active);
                    repos(tb,0);
                }
            }
            else if(mp.y >p.y+(ph*0.75) && a<b){
                sp->render(p.x,p.y-2.0+ph,tb->transform.scale.x,2.0,color);
                if(b>=0 && release)
                {
                    tbuttons.erase(tbuttons.begin()+b);
                    tbuttons.insert(tbuttons.begin()+a,active);
                    repos(tb,1);
                }
            }
            else if(heir){
                sp->render(p.x,p.y,tb->transform.scale.x,ph,vec3(0.2,0.9,0.6),0.4);
                if(b>=0 && release){
                    repos(tb,2);
                }
            }
        }
    }
    // EnD OF REPOSITION
  }
  
}
void Menu::repos(TextButton* tb, char pos)
{
//   printf(" reposition Default :| \n\n");   
}
