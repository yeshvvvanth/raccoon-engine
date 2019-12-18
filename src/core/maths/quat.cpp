#include "quat.h"
#include<math.h>
#define PI 3.14159265

quat::quat()
      : x(0), y(0), z(0), w(1)
{
}

quat::quat(const quat& quat)
{
      x = quat.x;
      y = quat.y;
      z = quat.z;
      w = quat.w;
}

quat::quat(const float* elements)
{
   memcpy(this->elements, elements, 4 * sizeof(float));
}


quat::quat(float x,float y, float z, float w)
      : x(x), y(y), z(z), w(w)
{
}
/// rotation quat from two directional vectors
quat::quat(vec3 u,vec3 v)
{
    vec3 a = u.Cross(v);
    quat q=quat(a,1.0);
    q.w = sqrt(u.SqMagnitude()*v.SqMagnitude()) + u.Dot(v);
    if(u==-v)
        *this=FromEulerAngles(vec3(0.0,-PI,0.0));
    else
        *this=q.Normalized();
}

quat::quat(float angle, vec3 axis)
{
    float s = sin(angle *(0.5));
    w=cos(angle *(0.5));
    x=axis.x*s;
    y=axis.y*s;
    z=axis.z*s;
}


quat::quat(const vec4& vec)
{
      x = vec.x;
      y = vec.y;
      z = vec.z;
      w = vec.w;
}

quat::quat(float scalar)
{
      x = scalar;
      y = scalar;
      z = scalar;
      w = scalar;
}

quat::quat(const vec3& xyz, float w)
{
      this->SetXYZ(xyz);
      this->w = w;
}

quat operator*(quat& q,const float num)
{
   q.x*=num;q.y*=num;q.z*=num;q.w*=num;
   return q;
}

quat quat::Identity()
{
      return quat(0.0f, 0.0f, 0.0f, 1.0f);
}

quat quat::FromEulerAngles(const vec3 angles)
{
  return RotationZ(angles.z)*RotationY(angles.y)*RotationX(angles.x);
}

quat& quat::operator =(const quat & quat)
{
    memcpy(this->elements, quat.elements,  4 * sizeof(float));
    return *this;
}

quat& quat::SetXYZ(const vec3 & vec)
{
      x = vec.x;
      y = vec.y;
      z = vec.z;
      return *this;
}

const vec3 quat::GetXYZ() const
{
      return vec3(x, y, z);
}

quat& quat::SetElem(Uchar idx, float value)
{
      *(&x + idx) = value;
      return *this;
}

float quat::GetElem(Uchar idx) const
{
      return *(&x + idx);
}

float quat::operator [](Uchar idx) const
{
      return *(&x + idx);
}

vec3 quat::GetAxis() const
{
      float x = 1.0f - w * w;
      if (x < 0.0000001f) // Divide by zero safety check
              return vec3::Right(); ///Xaxis

      float x2 = x * x;
      return GetXYZ() / x2;
}
/*
float clamp(float x,float a,float b){
  return (x>a&&x<b)?x:(x>b?b:a);
}*/
float quat::pitch()
{
  return atan2((2.0) * (y * z + w * x), w * w - x * x - y * y + z * z);
}
float quat::yaw()
{
  return asin(clamp((-2.0) * (x * z - w * y), (-1.0), (1.0)));
}
float quat::roll()
{
  return atan2((2.0) * (x * y + w * z), w * w + x * x - y * y - z * z);
}

vec3 quat::ToEulerAngles() const
{
    return vec3(atan2((2.0) * (y * z + w * x), w * w - x * x - y * y + z * z),
	    asin(clamp((-2.0) * (x * z - w * y), (-1.0), (1.0))),
	    atan2((2.0) * (x * y + w * z), w * w + x * x - y * y - z * z));
//     return  vec3(pitch(),yaw(),roll());
}
/*
const quat quat::operator+(const quat& quaternion) const
{
      return quat(x + quaternion.x, y + quaternion.y, z + quaternion.z, w + quaternion.w);
}*/

const quat quat::operator-(const quat& quaternion) const
{
      return quat(x - quaternion.x, y - quaternion.y, z - quaternion.z, w - quaternion.w);
}

const quat quat::operator*(float scalar) const
{
      return quat(x * scalar, y * scalar, z * scalar, w * scalar);
}

const quat quat::operator/(float scalar) const
{
      return quat(x / scalar, y / scalar, z / scalar, w / scalar);
}

const quat quat::operator-() const
{
      return quat(-x,-y, -z, -w);
}

bool quat::operator ==(const quat & quaternion) const
{
      return (x == quaternion.x) && (y == quaternion.y) && (z == quaternion.z) && (w == quaternion.w);
}

bool quat::operator !=(const quat & quaternion) const
{
      return !(*this == quaternion);
}

float quat::sqMag(const quat& quaternion)
{
      return   (quaternion.x * quaternion.x)
             + (quaternion.y * quaternion.y)
             + (quaternion.z * quaternion.z)
             + (quaternion.w * quaternion.w);
}

float Length(const quat& quaternion)
{
      return sqrt(quat::sqMag(quaternion));
}

const quat Normalize(const quat& quaternion)
{
      return quaternion * 1/sqrt(quat::sqMag(quaternion));
}

const quat quat::Rotation(const vec3& unitVec0, const vec3& unitVec1)
{
      float cosHalfAngleX2, recipCosHalfAngleX2;
      cosHalfAngleX2 = sqrt((2.0f * (1.0f + unitVec0.Dot(unitVec1))));
      recipCosHalfAngleX2 = (1.0f / cosHalfAngleX2);
      return quat((unitVec0.Cross(unitVec1) * recipCosHalfAngleX2), (cosHalfAngleX2 * 0.5f));
}

const quat quat::Rotation(float radians, const vec3 & unitVec)
{
      float angle = radians * 0.5f;
      return quat((unitVec * sin(angle)), cos(angle));
}

const quat quat::operator*(const quat& quaternion) const
{
      return Normalize(quat(
              (((w * quaternion.x) + (x * quaternion.w)) + (y * quaternion.z)) - (z * quaternion.y),
              (((w * quaternion.y) + (y * quaternion.w)) + (z * quaternion.x)) - (x * quaternion.z),
              (((w * quaternion.z) + (z * quaternion.w)) + (x * quaternion.y)) - (y * quaternion.x),
              (((w * quaternion.w) - (x * quaternion.x)) - (y * quaternion.y)) - (z * quaternion.z)
              ));
}

vec3 quat::Rotate(const quat quaternion, const vec3 vec)
{
      float tmpX, tmpY, tmpZ, tmpW;
      tmpX = (((quaternion.w * vec.x) + (quaternion.y * vec.z)) - (quaternion.z * vec.y));
      tmpY = (((quaternion.w * vec.y) + (quaternion.z * vec.x)) - (quaternion.x * vec.z));
      tmpZ = (((quaternion.w * vec.z) + (quaternion.x * vec.y)) - (quaternion.y * vec.x));
      tmpW = (((quaternion.x * vec.x) + (quaternion.y * vec.y)) + (quaternion.z * vec.z));
      return vec3(
              ((((tmpW * quaternion.x) + (tmpX * quaternion.w)) - (tmpY * quaternion.z)) + (tmpZ * quaternion.y)),
              ((((tmpW * quaternion.y) + (tmpY * quaternion.w)) - (tmpZ * quaternion.x)) + (tmpX * quaternion.z)),
              ((((tmpW * quaternion.z) + (tmpZ * quaternion.w)) - (tmpX * quaternion.y)) + (tmpY * quaternion.x))
              );
}

quat quat::Conjugate() const
{
      return quat(-x, -y, -z, w);
}

quat quat::inverse() const
{
    quat cr=*this;
    return cr.Conjugate()*(cr.x*cr.x+cr.y*cr.y+cr.z*cr.z+cr.w*cr.w);
}
const quat Select(const quat& quat0, const quat& quat1, bool select1)
{
      return quat(select1 ? quat1.x : quat0.x, select1 ? quat1.y : quat0.y, select1 ? quat1.z : quat0.z, select1 ? quat1.w : quat0.w);
}

float quat::Dot(const quat& other) const
{
      float result = (x * other.x);
      result = (result + (y * other.y));
      result = (result + (z * other.z));
      result = (result + (w * other.w));
      return result;
}
//////////////////////////////////////
quat operator+(const quat& q1, const quat& q2)
{
   return quat(q1.x+q2.x,q1.y+q2.y,q1.z+q2.z,q1.w+q2.w);
}


quat quat::Normalized() const
{
  return  *this/sqrt(x*x+y*y+z*z+w*w); 
}
/*static inline float quat::Dot(const quat& q1,const quat& q2){
    return q1.x*q2.x+q1.y*q2.y+q1.z*q2.z+q1.w*q2.w; 
}*/
quat quat::lerp(const quat &q1, const quat &q2, float t)
{ 
 return (q1*(1-t) + q2*t).Normalized(); 
}
quat quat::slerp(const quat &q1, const quat &q2, float t)
{
      quat q3;
      float dot = q1.x*q2.x+q1.y*q2.y+q1.z*q2.z+q1.w*q2.w; //  Dot(q1, q2)
      /*      dot = cos(theta)
              if (dot < 0), q1 and q2 are more than 90 degrees apart,
              so we can invert one to reduce spinning	*/
      if (dot < 0)
      {
           dot = -dot;
           q3 = -q2;
      } else q3 = q2;

      if (dot < 0.95f)
      {
          float angle = acos(dot);
          return (q1*sin(angle*(1-t)) + q3*sin(angle*t))/sin(angle);
      } else // if the angle is small, use linear interpolation
          return lerp(q1,q3,t);
}

