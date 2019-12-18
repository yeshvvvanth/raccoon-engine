#include"vec2.h"

vec2::vec2():x(0),y(0){}


vec2::vec2(float X, float Y):x(X),y(Y)
{
  
}
vec2 vec2::operator+(vec2 right)
{
    return vec2(x+right.x,y+right.y);
}
vec2 vec2::operator*(vec2 right)
{
    return vec2(x*right.x,y*right.y);
}
vec2 vec2::operator*(float num)
{
    return vec2(x*num,y*num);
}

vec2& vec2::operator=(vec2 right)
{
  this->x=right.x;
  this->y=right.y;
  
  return *this;
}

bool vec2::operator!=(vec2 right)
{
  return x!=right.x||y!=right.y;
}
bool vec2::operator==(vec2 right)
{
  return x==right.x&&y==right.y;
}

float clamp(float x,float min,float max)
{
  return (x>min&&x<max)?x:(x>max?max:min);
}
