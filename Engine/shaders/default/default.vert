#version 100
attribute vec3 position;
attribute vec4 boneIDs;
attribute vec4 weights;
attribute vec2 texcoord;
attribute vec3 Color;
attribute vec3 normal;
uniform mat4 PVM;
uniform mat4 model;
uniform mat4 norMat,lightMat;
uniform float rigged;// 0 if rigged
uniform mat4 anim[24];

varying vec2 Texcoord;
varying vec3 vColor;
varying vec4 lightFragPos;
varying vec3 vNormal;
varying vec3 FragPos;

void main()
{
    mat4 aniMatrix=mat4(1.0);
    if(rigged==1.0){
        aniMatrix= weights.x * anim[int(boneIDs.x)] +
                   weights.y * anim[int(boneIDs.y)] +
                   weights.z * anim[int(boneIDs.z)] +
                   weights.w * anim[int(boneIDs.w)] ;
    }
    gl_Position=(PVM*aniMatrix)* vec4(position,1.0);
     
    vColor=Color;
    Texcoord = texcoord;
    /// light stuff
    vNormal= ( (model*aniMatrix*rigged+(1.0-rigged)*norMat) *vec4(normal,0.0)).xyz;
//     FragPos=(model*aniMatrix*vec4(position,1.0)).xyz; 
//     FragPos=((model)*vec4 (  (aniMatrix*vec4(position,1.0)).xyz,1.0 )  ).xyz;
    vec4 tfp=model*aniMatrix*vec4 (position,1.0);
    FragPos=tfp.xyz/tfp.w;
    lightFragPos= (lightMat*model*aniMatrix*vec4 (position,1.0));

}
