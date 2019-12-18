#include"rawmesh.h"
#include<algorithm>
std::vector<RawMesh*> RawMesh::meshes;
std::vector<RawMesh*> RawMesh::emeshes;
RawMesh::RawMesh(unsigned char ty, std::ifstream& ifs,Shader* sh):type(ty)
{ 
 shader=sh;
 uvs=ccs=0;
 read(ifs,sh);
//  if(type&MESH_RIGGED || type&MESH_ANIM ){arm->rig(sh->PID);}
}
RawMesh::RawMesh()
{
    type=0;
    vertices=uvs=ccs=0;
    vao=vert_data=0;
}

void RawMesh::read(std::ifstream& ifs,Shader* sh){
  animate=false;
  arm=NULL;
  uvs=NULL;ccs=NULL;

  unsigned char n_len=0;
  char *tname=0;
  
  ifs.read((char*)&n_len,1);
  tname=new char[n_len+1];tname[n_len]='\0';
  ifs.read((char*)tname,n_len);
  printf("name of mesh=%s \n",tname);
  name=std::string(tname);
  delete []tname;
  ifs.read((char*)&es,4);
  unsigned int*telements=new unsigned int[es];
  ifs.read((char*)telements,es*4);
  indices=new unsigned short[es];
  std::copy(telements,telements+es,indices);
  delete []telements;
  ifs.read((char*)&vs,4);
  vertices=new float[vs*6];
  //float vertices[vs*6];
  ifs.read((char*)vertices,vs*6*4);
  
  if(type&MESH_TEX){
    uvs=new float[vs*2];
    ifs.read((char*)uvs,vs*2*4);
  }
  else{
    ccs=new float[vs*3];
    ifs.read((char*)ccs,vs*3*4);
  }
  vec3 Lbb,Ubb;
  
  ifs.read((char*)Lbb.elements,4*3);
  ifs.read((char*)Ubb.elements,4*3);
  
  box=Box(Lbb,Ubb);
  
  if(type&MESH_RIGGED){
    bdata=new float[vs*5];
    ifs.read((char*)bdata,vs*5*4);
  }
  if(type&MESH_RIGGED){arm=new Armature(ifs,type&MESH_ANIM);}   /// load armature
  
  set_shader(sh);                                           ///  set vertex attributes
}
  
void RawMesh::render(Transform& trans)
{ 
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  
  glUseProgram(shader->PID);

  glUniform1f( shader->uniRigged,float(bool(type&MESH_ANIM)));  
  glUniform1f( shader->uniTextured,float(bool(type&MESH_TEX)));  
    
  glBindVertexArray(vao);
  glUniformMatrix4fv(shader->uniModel, 1, GL_FALSE, (trans.model.elements));
  mat4 tmat=mat4::Transpose(mat4::Inverse(trans.model));
  glUniformMatrix4fv(glGetUniformLocation(shader->PID,"norMat"), 1, GL_FALSE, (tmat.elements));

  glDrawElements(GL_TRIANGLES,es , GL_UNSIGNED_SHORT, 0);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
}

RawMesh::~RawMesh()
{
 //GLuint vert_data,ebo,uv_data,col_data,bone_data;
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vert_data);
  
  if(type&MESH_TEX){glDeleteBuffers(1, &uv_data);delete[] uvs;}
  else if(ccs){glDeleteBuffers(1, &col_data);delete[] ccs;}
  
  if(type&MESH_RIGGED){glDeleteBuffers(1, &bone_data);delete[] bdata;}
  glDeleteVertexArrays(1, &vao);
  
  delete[] indices;
  delete[] vertices;
  
  if(type&MESH_RIGGED)delete arm; //
   
}

RawMesh * RawMesh::findRawMesh(std::string tname,bool e)
{
    std::vector<RawMesh*> *v;
    RawMesh* m=0;
    if(e){v=&emeshes;}
    else{v=&meshes;}
    
    for(int i=0;i<v->size();i++){
        if(tname==(*v)[i]->name ){m=(*v)[i];break;}
    }
    return m;
}

