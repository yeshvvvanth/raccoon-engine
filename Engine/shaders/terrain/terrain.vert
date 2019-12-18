#version 100
attribute vec3 position;
attribute vec3 normal;
uniform mat4 PVM;
varying vec2 Texcoord;
struct Light{
  vec3  col,dir,pos;
  float radius;
  int   type;
};
varying vec3 flight,lightDir,fnormal;
uniform mat4 model,lightMat,norMat;
uniform Light lights[5];
varying vec4 lightFragPos;
void main()
{
    gl_Position = PVM*vec4(position, 1.0);
    Texcoord=vec2(position.x,position.z);
    vec3 norm = normalize((norMat*vec4(normal,0.0)).xyz);
    flight=lights[4].col*max(dot(norm,lights[4].dir ), 0.0);
    lightFragPos= (lightMat*model*vec4 (position,1.0));
    lightDir=lights[4].dir;fnormal=norm;
}
