#version 100
precision mediump float;
varying vec2 Texcoord;
varying vec3 vColor;

// light stuff 
struct Light{
  vec3  col,dir,pos;
  float radius;
  int   type;
};
uniform sampler2D tex,shadowMap;
uniform vec2 tile,tileoff;
 uniform Light lights[5];
uniform float alpha,istex;
uniform samplerCube skybox,shadowCubeMap;
uniform vec3 viewPos;
uniform float smoothness;
uniform int doShadow;
varying vec4 lightFragPos;
varying vec3 vNormal;
varying vec3 FragPos;
uniform vec3 lightDir,ambient;

 vec3 CalcPointLight(int i);
 vec3 CalcDirectionalLight(int i);
 float Shadow(vec4 fpls,float fuku);   
void main()
{
    vec3 light=ambient;
    light += CalcDirectionalLight(4);
    /*
    */
    for(int i=0;i<4;i++){
        if(lights[i].type==1){
                light += CalcPointLight(i);
        }
    }
//      vec3 I = normalize(FragPos - viewPos);
//      vec3 R = reflect(I, normalize(vNormal));
//      float ratio = 1.00 / 1.52;
//      vec3 R = refract(I, normalize(vNormal), ratio);
//     vec4 scolor= textureCube(skybox, R)*0.4;
    float gamma = 2.2;
//     vec4 diffuse=texture2D(tex, Texcoord*tile+tileoff);
    vec4 diffuse= vec4(pow(texture2D(tex, Texcoord*tile+tileoff).rgb, vec3(gamma)),1.0);
    gl_FragColor =(vec4(vColor,1.0-istex)+istex*diffuse)* vec4(vec3(light),alpha);
//     gl_FragColor=mix(gl_FragColor,scolor,0.4);
    gl_FragColor.xyz = pow(gl_FragColor.xyz, vec3(1.0/gamma));
}

float Shadow(vec4 fragPosLightSpace,float fuku)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w ;
    // Transform to [0,1] range
    projCoords = (projCoords * 0.5)+ 0.5;
    float shadow=0.0;
    if(projCoords.z <= 1.0){
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture2D(shadowMap, projCoords.xy).x; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(vNormal,lightDir)), 0.005); 
    /*
    vec2 texelSize = 1.0 / vec2(2048,2048);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    */
    // Check whether current frag pos is in shadow
    shadow = currentDepth-bias > closestDepth  ? 1.0 : 0.0;
    }
    return 1.0-shadow;
} 
/*
float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    // Replace f and n with the far and near plane values you used when
    //   you drew your cube map.
    const float f = 10.0;
    const float n = 0.4;

    float NormZComp = (f+n) / (f-n) - (2.0*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}
*/
vec3 CalcPointLight(int i){
    Light light;
    for(int a=0;a<4;a++){
        if(a==i){light=lights[a];}
    }
    vec3 norm = normalize(vNormal);
    vec3 lightDir= normalize(light.pos- FragPos);
        
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3  diffuse = diff * light.col;
    float dist=length(light.pos-FragPos);
    
    
    float att = clamp(1.0 - dist/light.radius, 0.0, 1.0); 
    att *= att;
    //////specular////////
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    //blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0*smoothness);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0),10.0*smoothness);
    vec3 specular = smoothness * spec * light.col; 
    /*
    float tshadow=1.0;
    float SampledDistance = textureCube(shadowCubeMap,-lightDir).x;
    if (VectorToDepth(FragPos-light.pos)< SampledDistance+0.001)
        tshadow= 1.0; // Inside the light
    else
        tshadow= 0.2; // Inside the shadow
    */
    return (diffuse+specular)*att;
}
 vec3 CalcDirectionalLight(int i){
    Light light=lights[4];
    
    vec3 norm = normalize(vNormal);
    float fuku=dot(norm, light.dir );
    float diff = max(fuku, 0.0);
    vec3  diffuse = diff * light.col;
    //specular 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light.dir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),10.0*smoothness);
    diffuse += smoothness * spec * light.col; 
    
    if(doShadow>0)diffuse*=Shadow(lightFragPos,fuku);
    return diffuse;
 }
