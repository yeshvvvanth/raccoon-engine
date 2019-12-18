#include"animation.h"

Pbone::~Pbone(){delete[] keys;}

void Animation::read(T_BONE num_bones,std::ifstream& ifs)
{
  ifs.read((char*)&ID,sizeof(char));
  ifs.read((char*)&n_len,sizeof(char));
  char* tname=new char[n_len+1];
  ifs.read((char*)tname,n_len);tname[n_len]='\0';
  name=std::string(tname);
//   printf("animation (%s) \n",tname);
  delete []tname;
  ifs.read((char*)&fps,sizeof(char));
  ifs.read((char*)&num_frames,sizeof(short));    //number of frames in animation
  ifs.read((char*)&num_keys,sizeof(short)); 
  pbones=new Pbone[num_bones];

  for(unsigned int b=0;b<num_bones;b++){
      pbones[b].keys=new Key[num_keys];
      
      for(int k=0;k<num_keys;k++){
	Key& key= pbones[b].keys[k];
	ifs.read((char*)&key.frame_num,sizeof(T_BONE)); //frame number of keyframe
      }
      for(int k=0;k<num_keys;k++){
	Key& key= pbones[b].keys[k];
        float val[4];
        
	ifs.read((char*)val,3*sizeof(float));
	key.pos=vec3(val);
	ifs.read((char*)val,4*sizeof(float));
        key.rot=quat(val);
	ifs.read((char*)val,3*sizeof(float));
        key.scale=vec3(val);

      }
  }
}

Animation::~Animation(){
  delete[] pbones;
}
