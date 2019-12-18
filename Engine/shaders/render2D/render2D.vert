#version 100
attribute vec3 position;
attribute vec2 texcoord;
varying vec2 Texcoord;
uniform mat4 model;
uniform mat4 ortho;
void main()
{
   gl_Position = ortho*model*vec4(position, 1.0);
   Texcoord=texcoord; 
}
