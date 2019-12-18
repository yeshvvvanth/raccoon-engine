#include"vec3.h"
#include"mat4.h"

vec3::vec3():x(0),y(0),z(0){}
vec3::vec3(float X, float Y, float Z):x(X),y(Y),z(Z){}
vec3::vec3(float v):x(v),y(v),z(v){}

float& vec3::operator[](int i){return elements[i];}

vec3 vec3::Up(){return vec3(0.0f, 1.0f, 0.0f);}
vec3 vec3::Down(){return vec3(0.0f, -1.0f, 0.0f);}
vec3 vec3::Left(){return vec3(-1.0f, 0.0f, 0.0f);}
vec3 vec3::Right(){return vec3(1.0f, 0.0f, 0.0f);}
vec3 vec3::Zero(){return vec3(0.0f, 0.0f, 0.0f);}


vec3 vec3::Xaxis(){return vec3(1.0f, 0.0f, 0.0f);}
vec3 vec3::Yaxis(){return vec3(0.0f, 1.0f, 0.0f);}
vec3 vec3::Zaxis(){return vec3(0.0f, 0.0f, 1.0f);}

vec3& vec3::add(const vec3& other)
{
     x += other.x;
     y += other.y;
     z += other.z;

     return *this;
}

vec3& vec3::subtract(const vec3& other)
{
     x -= other.x;
     y -= other.y;
     z -= other.z;

     return *this;
}

vec3& vec3::multiply(const vec3& other)
{
     x *= other.x;
     y *= other.y;
     z *= other.z;

     return *this;
}

vec3& vec3::divide(const vec3& other)
{
     x /= other.x;
     y /= other.y;
     z /= other.z;

     return *this;
}

vec3::vec3(float* arr)
{
  memcpy(elements,arr,3*sizeof(float));
}


vec3 vec3::Multiply(const mat4& mat) const
{
  return vec3(
      mat.elements[0]*x+mat.elements[4]*y+mat.elements[8]*z+mat.elements[12]*1,
      mat.elements[1]*x+mat.elements[5]*y+mat.elements[9]*z+mat.elements[13]*1,
      mat.elements[2]*x+mat.elements[6]*y+mat.elements[10]*z+mat.elements[14]*1
  );   
}

vec3 operator+(vec3 left, const vec3& right)
{
  return left.add(right);
}

vec3& vec3::operator+=(vec3 other)
{
  return this->add(other);
}


vec3 operator-(vec3 left, const vec3& right)
{
  return left.subtract(right);
}

vec3 operator*(vec3 left, const vec3& right)
{
  return left.multiply(right);
}

vec3 operator/(vec3 left, const vec3& right)
{
  return left.divide(right);
}

bool vec3::operator==( vec3 other) 
{
   return x == other.x && y == other.y && z == other.z;
}

bool vec3::operator!=( vec3 other) 
{
   return !(*this == other);
}

vec3 operator-(const vec3& vector)
{
     return vec3(-vector.x, -vector.y, -vector.z);
}
/// vector fun
vec3 vec3::Cross(const vec3& other) const
{
    return vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

float vec3::Dot(const vec3& other) const
{
    return x * other.x + y * other.y + z * other.z;
}

float vec3::Magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}
float vec3::SqMagnitude() const
{
    return (x * x + y * y + z * z);
}

vec3 vec3::Normalize() const
{
    float length = Magnitude();
    if (length) 
        return vec3(x / length, y / length, z / length);
    else
        return vec3(x,y,z);
}

float vec3::Distance(const vec3& other) const
{
    float a = x - other.x;
    float b = y - other.y;
    float c = z - other.z;
    return sqrt(a * a + b * b + c * c);
}
float vec3::SqDistance(const vec3& other) const
{
    float a = x - other.x;
    float b = y - other.y;
    float c = z - other.z;
    return (a * a + b * b + c * c);
}
/////////////
vec3 vec3::lerp(const vec3& v1, const vec3& v2, float t)
{
   return (v1*(1-t) + v2*t);
}
