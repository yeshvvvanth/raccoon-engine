#version 100
attribute vec3 position;
attribute vec4 boneIDs;
attribute vec4 weights;
attribute vec2 texcoord;
attribute vec3 Color;
attribute vec3 normal;
struct Light{
  vec3  col,dir,pos;
  float radius;
  int   type;
};
uniform mat4 PVM;
uniform mat4 model;
uniform mat4 norMat,lightMat;
uniform float rigged;// 0 if rigged
uniform mat4 anim[24];
uniform Light lights[5];

varying vec2 Texcoord;
varying vec3 vColor;
varying vec4 lightFragPos;
varying vec3 fnormal;


// light stuff 
varying vec3 FragPos;

varying vec3 dlight;
varying vec3 flight,lightDir;
vec3 CalcPointLight(int i,vec3 norm);
vec3 CalcDirLight(int i,vec3 norm);
void main()
{
    mat4 aniMatrix=mat4(1.0);
    if(rigged==1.0){
        aniMatrix=  weights.x * anim[int(boneIDs.x)] +
                    weights.y * anim[int(boneIDs.y)] +
                    weights.z * anim[int(boneIDs.z)] +
                    weights.w * anim[int(boneIDs.w)] ;
    }
    gl_Position=(PVM*aniMatrix)* vec4(position,1.0);
     
    vColor=Color;
    Texcoord = texcoord;
    /// light stuff
    vec3 vNormal= ( (model*aniMatrix*rigged+norMat*(1.0-rigged)) *vec4(normal,0.0)).xyz;
//     FragPos=(model*aniMatrix*vec4 (position,1.0)  ).xyz; //doesn't work
//     FragPos=(model*vec4((aniMatrix*vec4 (position,1.0)  ).xyz,1.0)).xyz; //doesn't either
    vec4 tfp=model*aniMatrix*vec4 (position,1.0);
    FragPos=tfp.xyz/tfp.w;
    
    lightFragPos= (lightMat*model*aniMatrix*vec4 (position,1.0));
    
    vec3 norm = normalize(vNormal);
    fnormal=norm;
    float lin=1.0,quad=0.09,con=0.032;
    float range=2.0;
    flight = vec3(0.0);dlight=vec3(0.0);
    
    dlight += CalcDirLight(4,norm);
    for(int i=0;i<4;i++){
        if(lights[i].type==1)flight += CalcPointLight(i,norm);
    }
    lightDir=lights[4].dir;
}

vec3 CalcPointLight(int i,vec3 norm){
    Light light;
    for(int a=0;a<4;a++){
        if(a==i){light=lights[a];}
    }
    vec3 lightDir= normalize(light.pos- FragPos);
        
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3  diffuse = diff * light.col;
    float dist=length(light.pos-FragPos);
    
    float att = clamp(1.0 - dist/light.radius, 0.0, 1.0); 
    diffuse *= att*att;
    return diffuse;
}
 vec3 CalcDirLight(int i,vec3 norm){
    Light light=lights[4];
    
    float diff = max(dot(norm, light.dir ), 0.0);
    vec3  diffuse = diff * light.col;
    
    return diffuse;
 }
