#pragma once
class vec2{
public:
 union{struct{float x,y;};float elements[2];};
 vec2();
 vec2(float x,float y);
//  float& operator [] (int x);
 vec2& operator=(vec2 right); 
 vec2 operator+(vec2 right);
 vec2 operator*(vec2 right); 
 vec2 operator*(float num); 
 bool operator!=(vec2 right); 
 bool operator==(vec2 right); 
};


float clamp(float x,float min,float max);
#define Max(a,b) a<b?b:a
