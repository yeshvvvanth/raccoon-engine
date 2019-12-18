#include"armature.h"

Armature::Armature(std::ifstream& ifs,bool isanim):isAnimated(isanim)
{
  read(ifs); active_anim=0;transition=false;trf=1.0;past_anim=0;tf=0;
}
void Armature::read(std::ifstream& ifs)
{
  ifs.read((char*)&num_bones,sizeof(char));
  bones=new Bone[num_bones];
  for(int k=0;k<num_bones;k++)bones[k].read(ifs);
//   printf("number of bones=%d\n .....\n",num_bones);
  ////////////////////////////
  num_an=0;
  if(isAnimated){
    ifs.read((char*)&num_an,sizeof(char));
    animations.reserve(num_an);
    for(int i=0;i<num_an;i++){
        Animation* anim=new Animation;
        anim->read(num_bones,ifs);
        animations.push_back(anim);
    }
  }
 
}
Armature::~Armature(){
  delete[] bones;
  if(isAnimated){
      for(int i=0;i<animations.size();i++)
        delete animations[i];
  }
}

void Armature::rig(GLuint shader_ID){
    /*
 for(unsigned short h=0;h<num_bones;h++){
   mat4 trans=(bones[h].transform);      ///
   mat4 offm;
  
   if(bones[h].parentID!=255){
    mat4 pt=mat4(transforms+16*(bones[h].parentID));
     trans=pt*trans;
   }
   
    float* valv;
    valv=trans.elements;
    for(unsigned char g=0;g<16;g++){
      transforms[h*16+g]=valv[g];
    }
   
   if(bones[h].deform){
    offm=(bones[h].offset);
    trans=trans*offm;
    }
  
    float* val;
    val=trans.elements;
   for(unsigned char g=0;g<16;g++){
     matrices[h*16+g]=val[g];
   }
 }
 */
}


 
 
 
 

