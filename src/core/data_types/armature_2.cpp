#include"armature.h"
#include"../main/input.h"
#include"../render/transform.h"
Transform keyTrans(Pbone &pb,short key1,short key2,float framefactor);
void Armature::animate(GLuint shader_ID,int &f,float &ff,bool go)
{
 int num_keys=animations[active_anim]->num_keys;// number of keyframes
 if(shader_ID){
    if(num_keys>1){ 
        if(animations[active_anim]->num_keys>1||transition){
        ff+=8*aspeed*Input::deltaTime*Input::timescale;
        if(ff>=1.00f){f++;tf++;ff=0;}
        if(f>=animations[active_anim]->num_keys-1){f=0;}
        }
        if(transition&& past_anim<num_an){
            if(tf>=animations[past_anim]->num_keys-1){tf=0;}
            trf+=0.05*tspeed;
            if(trf>1)transition=false;     
        }
    }else{
        f=0;ff=0;
    }
 }

 for(unsigned short h=0;h<num_bones;h++){
  Pbone &pb=animations[active_anim]->pbones[h];
    int nf=f;
    if (num_keys>1){nf++;}//if number of keys>1 tween
    Transform tr=keyTrans(pb,f,nf,ff);
    if(transition&& past_anim<num_an &&animations[past_anim]->num_keys>1&&num_keys>1 ){ // fix it!
        nf=tf+1;//if(animations[past_anim]->num_keys>1)nf++;
        Transform ttr=keyTrans(animations[past_anim]->pbones[h],tf,nf,ff);
        tr.scale   =vec3::lerp (ttr.scale   ,tr.scale   ,trf);
        tr.rotation=quat::slerp(ttr.rotation,tr.rotation,trf);
        tr.position=vec3::lerp (ttr.position,tr.position,trf);
    }
    
    mat4 matr(1.0);
    
    matr=mat4::Scale(tr.scale)*matr;
    matr=mat4::Rotate(tr.rotation)*matr;
    
    matr.elements[12]=tr.position.x;
    matr.elements[13]=tr.position.y;
    matr.elements[14]=tr.position.z;

    
    if(bones[h].parentID!=255){
         mat4 parm=mat4(transforms+16*(bones[h].parentID));
         matr=parm*matr;
    }
    
    float* valv;
    valv=matr.elements;
    for(unsigned char g=0;g<16;g++){
      transforms[h*16+g]=valv[g];
    }
    
    if(bones[h].deform){
        mat4  offm=(bones[h].offset);
        matr=matr*offm;
    }
     
    float* val;
    val=matr.elements;
    for(unsigned char g=0;g<16;g++){
      matrices[h*16+g]=val[g];
    }
  }
}

Transform keyTrans(Pbone& pb, short key1, short key2, float framefactor)
{
  Transform tf;  
    tf.scale=vec3::lerp(pb.keys[key1].scale,pb.keys[key2].scale,framefactor);
    tf.rotation=quat::slerp(pb.keys[key1].rot,pb.keys[key2].rot,framefactor);
    tf.position=vec3::lerp(pb.keys[key1].pos,pb.keys[key2].pos,framefactor);
  return tf;
}

 
