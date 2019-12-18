#pragma once
#include<string.h>
struct quat;
#include"vec3.h"
#include"vec4.h"
struct mat4{                        // row+col*4
 //union{
   //float cols[4][4];
 //};
   float elements[4 * 4];
 mat4();
 mat4(float v);
 mat4(float* elements);
 
 float& operator [] (int i);
 mat4& set(float* data);
 ///Addition,subtraction
 mat4& Add(const mat4& other);
 mat4& Subtract(const mat4& other);
 mat4& operator+=(const mat4& other);
 mat4& operator-=(const mat4& other);
 friend mat4 operator+(mat4 left,const mat4& right);
 friend mat4 operator-(mat4 left,const mat4& right);
 ///multiplication:
    //matrix
 mat4& Multiply(const mat4& other);
 mat4& operator*=(const mat4& other);
 friend mat4 operator*(mat4 left,const mat4& right);
 friend vec3 operator*(const mat4& left, const vec3& right);
 friend vec4 operator*(const mat4& left, const vec4& right);
    //vector
 vec3 Multiply(const vec3& other) const;
 vec4 Multiply(const vec4& other) const;
 ///Transformations
 static mat4 Translate(const vec3& translation);
 static mat4 Scale(const vec3& scale);
 static mat4 Rotate(float angle, const vec3 axis);
 static mat4 Rotate(const quat& quaternion);
 /// Trans
 static mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
 static mat4 Perspective(float fov, float aspectRatio, float near, float far);
 static mat4 LookAt(const vec3& camera, const vec3& object, const vec3& up);
 ///const ops
 mat4 operator*(const float num);
 friend mat4 operator*(float num,mat4& mat);
 mat4& Invert();
 static mat4 Inverse(const mat4& matrix);
 static mat4 Transpose(const mat4& matrix);
 /*
 vec4 GetColumn(int index);
 

 */
};
