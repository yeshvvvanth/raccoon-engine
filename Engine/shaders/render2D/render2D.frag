#version 100
precision mediump float;
varying vec2 Texcoord;
uniform sampler2D tex;
uniform float alpha;
uniform vec3 tint;
uniform float fnt;
void main()
{   
    vec4 col=texture2D(tex, Texcoord);
    gl_FragColor =(col*(1.0-fnt)+vec4(1.0,1.0,1.0,col.r)*fnt)*vec4(tint,alpha);

    //  gl_FragColor =texture2D(tex, Texcoord)*vec4(tint,alpha);
}
