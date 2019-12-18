#include"rawmesh.h"


void RawMesh::set_shader(Shader* sh){
  shader=Shader::def;
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  ///indices
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned short)*es, indices, GL_STATIC_DRAW);


  ///vertices
  glGenBuffers(1, &vert_data);
  glBindBuffer(GL_ARRAY_BUFFER, vert_data);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vs*6, vertices, GL_STATIC_DRAW);
  //////////////////////////////////////////////////
    ///positions

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
    ///Normals
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)) );

  ///texture coordinates
  if(type&MESH_TEX){
      glGenBuffers(1, &uv_data);
      glBindBuffer(GL_ARRAY_BUFFER, uv_data);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vs*2, uvs, GL_STATIC_DRAW);

      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,0*sizeof(float), (void*)(0*sizeof(float)));

      //delete[] uvs;
  }
  ///colour coordinates
  else{
      glGenBuffers(1, &col_data);
      glBindBuffer(GL_ARRAY_BUFFER, col_data);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vs*3, ccs, GL_STATIC_DRAW);

      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,0*sizeof(float), (void*)(0*sizeof(float)));

      //delete[] ccs;
  }
  if(type&MESH_RIGGED){
      //bones
      glGenBuffers(1, &bone_data);
      glBindBuffer(GL_ARRAY_BUFFER,bone_data);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vs*5,bdata, GL_STATIC_DRAW);
      //boneids
//       glBindAttribLocation(shader->PID,1,"boneIDs");
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(boneAttrib, 4, GL_UNSIGNED_BYTE,GL_FALSE,5*sizeof(float), (void*)(0*sizeof(float)));
      //weights
//       glBindAttribLocation(shader->PID,2,"weights");
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(weightAttrib, 4, GL_FLOAT, GL_FALSE,5*sizeof(float), (void*)(1*sizeof(float)));

  }
   glBindVertexArray(0);
    this->shader= sh;
}

