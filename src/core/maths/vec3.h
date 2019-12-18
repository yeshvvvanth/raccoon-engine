#pragma once
struct mat4;
#include<string.h>
#include<math.h>
#include"vec2.h"
typedef unsigned char Uchar;
struct vec3{
 union{struct{float x,y,z;};float elements[3];};

 vec3();
 vec3(float value);
 vec3(float* arr);
 vec3(float X,float Y,float Z);
 float& operator [] (int i);
 
 static vec3 Up();
 static vec3 Down();
 static vec3 Left();
 static vec3 Right();
 static vec3 Zero();

 static vec3 Xaxis();
 static vec3 Yaxis();
 static vec3 Zaxis();
 vec3& add(const vec3& other);
 vec3& subtract(const vec3& other);
 vec3& multiply(const vec3& other);
 vec3& divide(const vec3& other);

 friend vec3 operator+(vec3 left, const vec3& right);
 friend vec3 operator-(vec3 left, const vec3& right);
 friend vec3 operator*(vec3 left, const vec3& right);
 friend vec3 operator/(vec3 left, const vec3& right);

 vec3 Multiply(const mat4& transform)const ;

 friend vec3 operator-(const vec3& vector);
 bool operator==( vec3 other) ;
 bool operator!=( vec3 other) ;

 vec3& operator+=( vec3 other);
 vec3& operator-=( vec3 other);
 vec3& operator*=( vec3& other);
 vec3& operator/=( vec3& other);

 vec3 Cross(const vec3& other) const;
 float Dot(const vec3& other) const;

 vec3 Normalize() const;
 float Magnitude() const;
 float Distance(const vec3& other) const;
 float SqMagnitude() const;
 float SqDistance(const vec3& other) const;
 static vec3 lerp(const vec3 &v1, const vec3 &v2, float t);
};


