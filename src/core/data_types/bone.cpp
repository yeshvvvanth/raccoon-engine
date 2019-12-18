#include"bone.h"

void Bone::read(std::ifstream& ifs){
  ifs.read((char*)&ID,sizeof(char));                                     // node id
  ifs.read((char*)&deform,sizeof(char));                            // boolean if deform
  unsigned char n_len;
  ifs.read((char*)&n_len,sizeof(char));                             // length of name of node
  char* tname=new char[n_len+1];tname[n_len]='\0';   
  ifs.read((char*)tname,(n_len));name.assign(tname);                            // name of node 
  delete[] tname;
  ifs.read((char*)&parentID,sizeof(char));
  ifs.read((char*)&ch_num,sizeof(char));                       // number of children of node
  children=new unsigned char[ch_num]; 
  if(ch_num>0){
      ifs.read((char*)children,sizeof(unsigned char)*ch_num);      // node child ids
  }
  float val[16];
  ifs.read((char*)val,sizeof(float)*16);                        //node transform matrix 
  transform= mat4(val);
  if(deform){
      ifs.read((char*)val,sizeof(float)*16);                        //node transform matrix 
      offset= mat4(val);                                        // node offset matrix  
                             
  }
  
}
Bone::~Bone(){
  delete[] children;
}
