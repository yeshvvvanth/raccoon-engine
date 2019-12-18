#version 100
attribute vec3 position;
attribute vec4 boneIDs;
attribute vec4 weights;
uniform mat4 PVM;
uniform float rigged;// 0 if rigged
uniform mat4 anim[24];

void main()
{
    mat4 aniMatrix= weights.x * anim[int(boneIDs.x)] +
                    weights.y * anim[int(boneIDs.y)] +
                    weights.z * anim[int(boneIDs.z)] +
                    weights.w * anim[int(boneIDs.w)] ;
    aniMatrix=rigged*aniMatrix+(1.0-rigged)*mat4(1.0);
    
    gl_Position = (PVM*aniMatrix)*vec4(position, 1.0);
}
