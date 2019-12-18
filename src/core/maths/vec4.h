#pragma once
struct mat4;
#include<math.h>
#include"vec3.h"
struct vec4{

 union{struct{float x,y,z,w;};float elements[4];};
 vec4();
 vec4(float value);
 vec4(float X,float Y,float Z,float W);
 vec4(const vec3& xyz, float w);
 float& operator [] (int i);
 
 vec4& Add(const vec4& other);
 vec4& Subtract(const vec4& other);
 vec4& Multiply(const vec4& other);
 vec4& Divide(const vec4& other);

 vec4 Multiply(const mat4& transform) const;///////

 friend vec4 operator+(vec4 left, const vec4& right);
 friend vec4 operator-(vec4 left, const vec4& right);
 friend vec4 operator*(vec4 left, const vec4& right);
 friend vec4 operator/(vec4 left, const vec4& right);

 bool operator==(const vec4& other);
 bool operator!=(const vec4& other);

 vec4& operator+=(const vec4& other);
 vec4& operator-=(const vec4& other);
 vec4& operator*=(const vec4& other);
 vec4& operator/=(const vec4& other);

 float Dot(const vec4& other);

 
};
