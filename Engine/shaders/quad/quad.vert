#version 100
attribute vec3 position;
attribute vec2 texcoord;

uniform mat4 PVM;
uniform mat4 model;
uniform float grad;

varying vec2 Texcoord;

void main()
{
    gl_Position = PVM*vec4(position, 1.0);
    Texcoord = texcoord;
}
