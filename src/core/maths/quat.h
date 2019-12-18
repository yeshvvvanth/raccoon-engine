
#include "mat4.h"
typedef unsigned char Uchar;

struct quat
{
       union{struct{float w,x,y,z;};float elements[4];};

       quat();
       quat(const quat& quaternion);
       quat(const float* elements);
       quat(float x, float y, float z, float w);
       quat( vec3 u, vec3 v);
       quat(const vec3& xyz, float w);
       quat(const vec4& vec);
       quat(float scalar);
       quat(float angle,vec3 axis); 
       
       
       friend quat operator*(float num,quat& q);

       quat& operator=(const quat& _quat); ///

       quat& SetXYZ(const vec3& vec);
       const vec3 GetXYZ() const;
       quat& SetElem(Uchar idx, float value);
       float GetElem(Uchar idx) const;
       vec3 GetAxis() const;
       vec3 ToEulerAngles() const;
       float pitch();
       float yaw();
       float roll();
       friend quat operator+(const quat& q1,const quat& q2);
       //const quat operator+(const quat& quaternion) const;
       const quat operator-(const quat& quaternion) const;
       const quat operator*(const quat& quaternion) const;
       const quat operator*(float scalar) const;
       const quat operator/(float scalar) const;
       float operator[](Uchar idx) const;

       quat& operator+=(const quat& quaternion)
       {
               *this = *this + quaternion;
               return *this;
       }

       quat& operator-=(const quat& quaternion)
       {
               *this = *this - quaternion;
               return *this;
       }

       quat& operator*=(const quat& quaternion)
       {
               *this = *this * quaternion;
               return *this;
       }

       quat& operator*=(float scalar)
       {
               *this = *this * scalar;
               return *this;
       }

       quat& operator/=(float scalar)
       {
               *this = *this / scalar;
               return *this;
       }

       const quat operator-() const;
       bool operator==(const quat& quaternion) const;
       bool operator!=(const quat& quaternion) const;
       static quat Identity();
       static quat FromEulerAngles(const vec3 angles);

       static vec3 Rotate(const quat  _quat, const vec3 vec);

       static const quat Rotation(const vec3& unitVec0, const vec3& unitVec1);
       static const quat Rotation(float radians, const vec3& unitVec);

       static const quat RotationX(float radians)
       {
               float angle = radians * 0.5f;
               return quat(sin(angle), 0.0f, 0.0f, cos(angle));
       }

       static const quat RotationY(float radians)
       {
               float angle = radians * 0.5f;
               return quat(0.0f, sin(angle), 0.0f, cos(angle));
       }

       static const quat RotationZ(float radians)
       {
               float angle = radians * 0.5f;
               return quat(0.0f, 0.0f, sin(angle), cos(angle));
       }
       quat Normalized()const;
       float Dot(const quat& other) const;
       static float sqMag(const quat& quaternion);
       
       static quat slerp(const quat &q1, const quat &q2, float t);
       static quat lerp(const quat &q1, const quat &q2, float t);
       quat Conjugate() const;
       quat inverse() const;
};


