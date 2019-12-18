#include"../data_types/shader.h"
#include"input.h"

bool Input::keys[318];
bool Input::pkeys[318];
float Input::px=0;
float Input::py=0;
vec2 Input::mousepos;
bool Input::mbuttons[32];
bool Input::pmbuttons[32];
short Input::textCurPos=0,Input::scroll=0,Input::pscroll=0,Input::scrollx;
float Input::deltaTime=0.02,Input::time=0,Input::ptime=0,Input::timescale=1.0;
bool Input::arrows[2];
bool Input::num=0;

bool Input::typing=false;
std::string Input::text="";

char tchar[2]={0,'\0'};

void Input::tfn(GLFWwindow* window, unsigned int kc)
{
   tchar[0]=(char)kc;
   if(typing/* &&(!num ||kc==45||kc==46||(kc>47 && kc<58) )*/)
	{text.insert(textCurPos,tchar);textCurPos++;}
}

void Input::kfn(GLFWwindow* win, int key, int scancode, int action, int mods)
{
  if(action==GLFW_PRESS){
    keys[key-31]=true;
  }
  else if(action==GLFW_RELEASE){
    keys[key-31]=false;
  }
  
  if(action==GLFW_PRESS){
   if(typing && key==KEY_BACKSPACE &&textCurPos>0){textCurPos--;text.erase (text.begin()+textCurPos);}
   if(typing && (key==KEY_ENTER||key==KEY_KP_ENTER)){typing=false;num=false;}
  }
  if(action==GLFW_REPEAT && typing){	
      if(key==KEY_BACKSPACE &&textCurPos>0){textCurPos--;text.erase (text.begin()+textCurPos);}
      if(key==KEY_RIGHT){arrows[1]=true;}
      if(key==KEY_LEFT){arrows[0]=true;}
  }
}
void Input::mbfn(GLFWwindow* win, int button, int action, int mod)
{
  if(action==GLFW_PRESS){
    mbuttons[button]=true;
  }
   else if(action==GLFW_RELEASE){
     mbuttons[button]=false;
  }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Input::scroll=yoffset;Input::scrollx=xoffset;
}
void Input::mposfn(GLFWwindow* win, double X, double Y)
{
    mousepos.x=X;
    mousepos.y=Y;
}

Input::Input()
{
  memset(keys,0,318);
  memset(pkeys,0,318);
  memset(mbuttons,0,32);
  memset(pmbuttons,0,32);
  arrows[0]=0;arrows[1]=0;
}


void Input::setWindow(GLFWwindow* win)
{
   glfwSetKeyCallback(win,kfn);
   glfwSetCursorPosCallback(win,mposfn);
   glfwSetMouseButtonCallback(win,mbfn);
   glfwSetCharCallback(win,tfn);
   glfwSetScrollCallback(win,scroll_callback);
}

bool Input::getKey(short unsigned int k)
{
   return keys[k-31];
}
bool Input::getKeyDown(short unsigned int k)
{
  return keys[k-31] && !pkeys[k-31];;
}

bool Input::getKeyDown(unsigned short mod,unsigned short k)
{
  return  keys[mod-31]&&(keys[k-31] && !pkeys[k-31]);
}

bool Input::getMouseButton(short unsigned int b)
{
   return  mbuttons[b];
}
bool Input::getMouseButtonDown(short unsigned int b)
{
   return  mbuttons[b] && !pmbuttons[b];
}

float Input::getMouseX()
{
  return mousepos.x;
}
float Input::getMouseY()
{
  return mousepos.y;
}

vec2 Input::getMousedelta()
{
  
  return vec2(mousepos.x-px,mousepos.y-py);
}

void Input::reset()
{
  time=glfwGetTime();  
  deltaTime=time-ptime;ptime=time;
  memcpy(pkeys,keys,sizeof(pkeys));
  memcpy(pmbuttons,mbuttons,sizeof(mbuttons));
  px=mousepos.x;py=mousepos.y;pscroll=scroll;
  arrows[0]=0;arrows[1]=0;scroll=0;
}

