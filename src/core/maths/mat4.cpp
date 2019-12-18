#include"mat4.h"
#include"quat.h"

typedef  unsigned char Uchar;

float& mat4::operator[](int i){return elements[i];}
///constructors
mat4::mat4()
{
  memset(elements, 0, 4 * 4 * sizeof(float));
   elements[0 + 0 * 4] = 1;
   elements[1 + 1 * 4] = 1;
   elements[2 + 2 * 4] = 1;
   elements[3 + 3 * 4] = 1;
}
mat4::mat4(float diagonal)
{
  memset(elements, 0, 4 * 4 * sizeof(float));
  elements[0 + 0 * 4] = diagonal;
  elements[1 + 1 * 4] = diagonal;
  elements[2 + 2 * 4] = diagonal;
  elements[3 + 3 * 4] = diagonal;
}
mat4::mat4(float* elements)
{
  memcpy(this->elements, elements, 4 * 4 * sizeof(float));
}
mat4& mat4::set(float* data){
    memcpy(this->elements, data, 4 * 4 * sizeof(float));
    return *this;
}

mat4 mat4::operator*(const float num)
{
     for(Uchar i=0;i<16;i++)
        this->elements[i]*=num;
    return *this;
}
mat4 operator*(const float num,mat4& mat)
{
     for(Uchar i=0;i<16;i++)
        mat.elements[i]*=num;
       return mat;
}
///Multiplication
mat4& mat4::Multiply(const mat4& other)
{
    float data[16];
    for (Uchar row = 0; row < 4; row++){
     for (Uchar col = 0; col < 4; col++){
           float sum = 0.0f;
           for (Uchar e = 0; e < 4; e++)
           {
              sum += elements[row+ e*4] * other.elements[e+col*4];
           }
           data[row + col * 4] = sum;
    }}
    memcpy(elements, data, 4 * 4 * sizeof(float));
    return *this;
}

mat4& mat4::operator*=(const mat4& right)
{
        return Multiply(right);
}
mat4 operator*(mat4 left, const mat4& right)
{
       return left.Multiply(right);
}
/// Addition ,Subtraction..
mat4& mat4::Add(const mat4& other)
{
    for(Uchar u=0;u<16;u++){
      elements[u]+=other.elements[u];
    }
    return *this;
}
mat4& mat4::Subtract(const mat4& other)
{
    for(Uchar u=0;u<16;u++){
      elements[u]-=other.elements[u];
    }
    return *this;
}
mat4& mat4::operator+=(const mat4& right)
{
        return Add(right);
}
mat4& mat4::operator-=(const mat4& right)
{
        return Subtract(right);
}
mat4 operator+(mat4 left, const mat4& right)
{
   return left.Add(right);
}
mat4 operator-(mat4 left, const mat4& right)
{
   return left.Subtract(right);
}

/// Transformations
//Translate:
mat4 mat4::Translate(const vec3& translation)
{
  mat4 matrix(1);
  matrix.elements[12]+=translation.x;
  matrix.elements[13]+=translation.y;
  matrix.elements[14]+=translation.z;
  return matrix;
}
//Scale:
mat4 mat4::Scale( const vec3& scale)
{
   mat4 matrix(1);
  matrix.elements[0]*=scale.x;
  matrix.elements[5]*=scale.y;
  matrix.elements[10]*=scale.z;
  return matrix;
}

mat4 mat4::Rotate(const quat& quaternion)
{
      mat4 result(1.0f);

      float qx, qy, qz, qw, qx2, qy2, qz2, qxqx2, qyqy2, qzqz2, qxqy2, qyqz2, qzqw2, qxqz2, qyqw2, qxqw2;
      qx = quaternion.x;
      qy = quaternion.y;
      qz = quaternion.z;
      qw = quaternion.w;
      qx2 = (qx + qx);
      qy2 = (qy + qy);
      qz2 = (qz + qz);
      qxqx2 = (qx * qx2);
      qxqy2 = (qx * qy2);
      qxqz2 = (qx * qz2);
      qxqw2 = (qw * qx2);
      qyqy2 = (qy * qy2);
      qyqz2 = (qy * qz2);
      qyqw2 = (qw * qy2);
      qzqz2 = (qz * qz2);
      qzqw2 = (qw * qz2);

      
      result.elements[0]=(1.0f - qyqy2) - qzqz2;
      result.elements[1]=(qxqy2 + qzqw2);
      result.elements[2]=(qxqz2 - qyqw2);
      result.elements[4]=(qxqy2 - qzqw2);
      result.elements[5]=((1.0f - qxqx2) - qzqz2);
      result.elements[6]=(qyqz2 + qxqw2);
      result.elements[8]=(qxqz2 + qyqw2);
      result.elements[9]=(qyqz2 - qxqw2);
      result.elements[10]=((1.0f - qxqx2) - qyqy2);
     
      return result;
}
/// 
mat4 mat4::Orthographic(float left, float right, float bottom, float top, float near, float far)
{
    mat4 result(1.0f);

    result.elements[0] = 2.0f / (right - left);
    result.elements[5] = 2.0f / (top - bottom);
    result.elements[10] = 2.0f / (near - far);

    result.elements[12] = (left + right) / (left - right);
    result.elements[13] = (bottom + top) / (bottom - top);
    result.elements[14] = (far + near) / ( near -far);

    return result;
}

mat4 mat4::Perspective(float fov, float aspectRatio, float near, float far)
{
    mat4 result(0.0f);

    float q = 1.0f / tan(0.5f*fov);
    float a = q * (1 / aspectRatio);

    float b = (near + far) / (near - far);
    float c = (2.0f * near * far) / (near - far);

    result.elements[0 + 0 * 4] = a;
    result.elements[1 + 1 * 4] = q;
    result.elements[2 + 2 * 4] = b;
    result.elements[3 + 2 * 4] = -1.0f;
    result.elements[2 + 3 * 4] = c;

    return result;
}
mat4 mat4::LookAt(const vec3& camera, const vec3& object, const vec3& up)
{
     mat4 result;
     vec3 f = (object - camera).Normalize();
     vec3 s = f.Cross(up).Normalize();
     vec3 u = s.Cross(f);

     result.elements[0 + 0 * 4] = s.x;
     result.elements[0 + 1 * 4] = s.y;
     result.elements[0 + 2 * 4] = s.z;

     result.elements[1 + 0 * 4] = u.x;
     result.elements[1 + 1 * 4] = u.y;
     result.elements[1 + 2 * 4] = u.z;

     result.elements[2 + 0 * 4] = -f.x;
     result.elements[2 + 1 * 4] = -f.y;
     result.elements[2 + 2 * 4] = -f.z;
     
     result.elements[0 + 3 * 4] = -s.Dot(camera);
     result.elements[1 + 3 * 4] = -u.Dot(camera);
     result.elements[2 + 3 * 4] = f.Dot(camera);

     return result;
}

///
vec4 mat4::Multiply(const vec4& other) const
{
        return other.Multiply(*this);
}
vec3 mat4::Multiply(const vec3& other) const
{
        return other.Multiply(*this);
}
vec3 operator*(const mat4& left, const vec3& right)
{
        return left.Multiply(right);
}

vec4 operator*(const mat4& left, const vec4& right)
{
        return left.Multiply(right);
}

mat4 mat4::Rotate(float angle, const vec3 axis)
{
       mat4 result(1.0f);

       float r = (angle)*M_PI/180.0f;
       float c = cos(r);
       float s = sin(r);
       float omc = 1.0f - c;
       
       float x = axis.x;
       float y = axis.y;
       float z = axis.z;

       result.elements[0 + 0 * 4] = x * omc + c;
       result.elements[1 + 0 * 4] = y * x * omc + z * s;
       result.elements[2 + 0 * 4] = x * z * omc - y * s;

       result.elements[0 + 1 * 4] = x * y * omc - z * s;
       result.elements[1 + 1 * 4] = y * omc + c;
       result.elements[2 + 1 * 4] = y * z * omc + x * s;

       result.elements[0 + 2 * 4] = x * z * omc + y * s;
       result.elements[1 + 2 * 4] = y * z * omc - x * s;
       result.elements[2 + 2 * 4] = z * omc + c;
       
       return result;
}
/////////////////////////////////////////////////////////////////////////////
mat4 mat4::Transpose(const mat4& mat){
    
 mat4 tmat=mat;   
    for(Uchar u=0;u<4;u++){
        for(Uchar v=0;v<4;v++){
            tmat.elements[u+4*v]=mat.elements[v+4*u];
        }
    }
    return tmat;
}

/////////////////////////////////////////////////////////////////////////////

static inline bool Invert2(float *mat, float *dst)
{
 float tmp[12]; /* temp array for pairs */
 float src[16]; /* array of transpose source matrix */
 float det; /* determinant */
 /* transpose matrix */
 for (int i = 0; i < 4; i++) {
    src[i] = mat[i*4];
    src[i + 4] = mat[i*4 + 1];
    src[i + 8] = mat[i*4 + 2];
    src[i + 12] = mat[i*4 + 3];
 }
 /* calculate pairs for first 8 elements (cofactors) */
 tmp[0] = src[10] * src[15];
 tmp[1] = src[11] * src[14];
 tmp[2] = src[9] * src[15];
 tmp[3] = src[11] * src[13];
 tmp[4] = src[9] * src[14];
 tmp[5] = src[10] * src[13];
 tmp[6] = src[8] * src[15];
 tmp[7] = src[11] * src[12];
 tmp[8] = src[8] * src[14];
 tmp[9] = src[10] * src[12];
 tmp[10] = src[8] * src[13];
 tmp[11] = src[9] * src[12];
 /* calculate first 8 elements (cofactors) */
 dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
 dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
 dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
 dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
 dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
 dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
 dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
 dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
 dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
 dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
 dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
 dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
 dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
 dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
 dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
 dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
 /* calculate pairs for second 8 elements (cofactors) */
 tmp[0] = src[2]*src[7];
 tmp[1] = src[3]*src[6];
 tmp[2] = src[1]*src[7];
 tmp[3] = src[3]*src[5];
 tmp[4] = src[1]*src[6];
 tmp[5] = src[2]*src[5];

 tmp[6] = src[0]*src[7];
 tmp[7] = src[3]*src[4];
 tmp[8] = src[0]*src[6];
 tmp[9] = src[2]*src[4];
 tmp[10] = src[0]*src[5];
 tmp[11] = src[1]*src[4];
 /* calculate second 8 elements (cofactors) */
 dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
 dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
 dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
 dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
 dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
 dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
 dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
 dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
 dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
 dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
 dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
 dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
 dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
 dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
 dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
 dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
 /* calculate determinant */
 det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
 /* calculate matrix inverse */
 if(det==0){return false;}
 det = 1/det;
 for (int j = 0; j < 16; j++)
    dst[j] *= det;
 return true;
}

mat4& mat4::Invert()
{
    float *a=elements;
    float inv[16];
    Invert2(a,inv);
    this->set(inv);
    return *this;
}
mat4 mat4::Inverse(const mat4& mat){
    mat4 tmat=mat;
    tmat.Invert();
    return tmat;
}
