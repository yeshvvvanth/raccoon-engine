#version 100
precision mediump float;
uniform sampler2D tex;
varying vec2 Texcoord;
uniform vec2 tile,tileoff;
uniform float alpha;
void main()
{
    gl_FragColor =texture2D(tex, Texcoord*tile+tileoff)*vec4(1.0,1.0,1.0,alpha);
}
