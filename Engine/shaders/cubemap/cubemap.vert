#version 100
attribute vec3 position;
varying vec3 textCoord;
uniform mat4 PVM;

void main()
{
    gl_Position = PVM*vec4(position, 1.0);
    textCoord=position;
}
