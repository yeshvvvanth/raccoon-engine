#include"vec4.h"
#include"mat4.h"

vec4::vec4():x(0),y(0),z(0),w(0){}
vec4::vec4(float X, float Y, float Z,float W):x(X),y(Y),z(Z),w(W){}
vec4::vec4(float v):x(v),y(v),z(v),w(v){}
vec4::vec4(const vec3& xyz, float w): x(xyz.x), y(xyz.y), z(xyz.z), w(w){}

float& vec4::operator[](int i){return elements[i];}

vec4& vec4::Add(const vec4& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

vec4& vec4::Subtract(const vec4& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

vec4& vec4::Multiply(const vec4& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;

    return *this;
}

vec4& vec4::Divide(const vec4& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;

    return *this;
}

vec4 vec4::Multiply(const mat4& mat) const
{
    return vec4(
      mat.elements[0]*x+mat.elements[4]*y+mat.elements[8]*z+mat.elements[12]*w,
      mat.elements[1]*x+mat.elements[5]*y+mat.elements[9]*z+mat.elements[13]*w,
      mat.elements[2]*x+mat.elements[6]*y+mat.elements[10]*z+mat.elements[14]*w,
      mat.elements[3]*x+mat.elements[7]*y+mat.elements[11]*z+mat.elements[15]*w
    );
}

vec4 operator+(vec4 left, const vec4& right)
{
    return left.Add(right);
}

vec4 operator-(vec4 left, const vec4& right)
{
    return left.Subtract(right);
}

vec4 operator*(vec4 left, const vec4& right)
{
    return left.Multiply(right);
}

vec4 operator/(vec4 left, const vec4& right)
{
    return left.Divide(right);
}

vec4& vec4::operator+=(const vec4& other)
{
    return Add(other);
}

vec4& vec4::operator-=(const vec4& other)
{
    return Subtract(other);
}

vec4& vec4::operator*=(const vec4& other)
{
    return Multiply(other);
}

vec4& vec4::operator/=(const vec4& other)
{
    return Divide(other);
}

bool vec4::operator==(const vec4& other)
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool vec4::operator!=(const vec4& other)
{
    return !(*this == other);
}

float vec4::Dot(const vec4& other)
{
    return x * other.x + y * other.y + z * other.z + w * other.w;
}
