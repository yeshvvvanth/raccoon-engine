#version 100
attribute vec3 position;
uniform mat4 PVM;
uniform vec4 color;
uniform vec4 color2;
uniform float grad;
varying vec4 colour;
void main()
{
    gl_Position = PVM*vec4(position, 1.0);
    colour=color *(1.0-grad) + grad*( (1.0-position.y)*color*0.5 + color2 *(1.0+position.y)*0.5 );
}
