#version 100
precision mediump float;
varying vec2 Texcoord;
varying vec3 vColor;
varying vec4 lightFragPos;
varying vec3 fnormal;
uniform sampler2D tex,shadowMap;
uniform vec2 tile,tileoff;
uniform float alpha,istex;
uniform int doShadow;
uniform vec3 ambient,glow;
varying vec3 flight,dlight,lightDir;
float ShadowCalculation(vec4 fpls);   
void main()
{ 
    float shadow=1.0;
    if(doShadow>0)shadow=(1.0-ShadowCalculation(lightFragPos));
    vec3 light=ambient+flight+dlight*shadow;
    float gamma = 2.2;
    vec4 diffuse= pow(texture2D(tex, Texcoord*tile+tileoff), vec4(gamma));
//     vec4 diffuse= texture2D(tex, Texcoord*tile+tileoff);
    gl_FragColor =(vec4(vColor,1.0-istex)+istex*diffuse ) * vec4(light,alpha)+vec4(glow,0.0);
    gl_FragColor.xyz = pow(gl_FragColor.xyz, vec3(1.0/gamma));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w ;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5+ 0.5;
    float shadow=0.0;
    if(projCoords.z <= 1.0){
        // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture2D(shadowMap, projCoords.xy).x; 
        // Get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;
        
//         float bias=0.0;
        float bias = max(0.01 * (1.0 - dot(fnormal,lightDir)), 0.005); 
//         shadow = currentDepth-bias > closestDepth  ? 1.0 : 0.0;
        
        //percentage closer filtering
        vec2 texelSize = 1.0 / vec2(1024,1024);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
        
    }
    return shadow;
} 
