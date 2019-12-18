#include"Mesh.h"
#include"sprite.h"
#include"camera.h"
#include"../main/input.h"
#include<BulletCollision/CollisionShapes/btShapeHull.h>
#ifdef FEMSCRIPTEN
#include<bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#endif
Renderable::~Renderable()
{
}


Mesh::Mesh(RawMesh* trawmesh, Material* tmaterial):Renderable()
{
    rawmesh=trawmesh;
    texture=0;
    box=rawmesh->box;
    animSpeed=transSpeed=1.0;
    anum=0;f=ff=0;
    if(rawmesh->type&MESH_ANIM)setAnimation(0);
    tile=vec2(1,1);tileoff=vec2(0,0);
    type=1;smoothness=(0.5);flags=DOSHADOW|DODEPTH;
}

void Mesh::setAnimation(int num, std::string tname)
{
    int tnum=-1;
    if(num<0){
        for(int i=0;i<rawmesh->arm->num_an;i++){
            if(rawmesh->arm->animations[i]->name==tname){tnum=i;break;}
        }
    }
    else if(num<rawmesh->arm->num_an){
        tnum=num;
    }
    if(tnum>-1 && (anum!=tnum||rawmesh->animate==false)){
        rawmesh->arm->past_anim=anum;
        rawmesh->arm->tf=f;
        anum=tnum;rawmesh->animate=true;
        rawmesh->arm->transition=true;
        rawmesh->arm->trf=0.0;
        rawmesh->arm->aspeed=animSpeed;
        rawmesh->arm->tspeed=transSpeed;
    }
}

void Mesh::tick(Transform& trans,float delta)
{
    if(flags&UNSHADED)rawmesh->shader=Shader::findShader("quad");
 
    if(texture&&rawmesh->type&MESH_TEX){texture->bind();}
    glUniform2fv(rawmesh->shader->uniTile,1,tile.elements);
    glUniform2fv(rawmesh->shader->uniTileOff,1,tileoff.elements);
    if(rawmesh->type&MESH_ANIM){rawmesh->arm->active_anim=anum;}
    if(rawmesh->type&MESH_RIGGED && rawmesh->animate){
        if(rawmesh->type&MESH_ANIM && rawmesh->animate){
            rawmesh->arm->animate(rawmesh->shader->PID,f,ff);
        }
        glUniformMatrix4fv(rawmesh->shader->uniAnim,maxbones,GL_FALSE,rawmesh->arm->matrices);
    }
    glUniform3fv(glGetUniformLocation(Shader::pervert->PID,"glow"),1,glow.elements);
}

Quad::Quad()
{
    texture=0;
    tile=vec2(1,1);tileoff=vec2(0,0);type=2;
    box=Box(vec3(-0.5,0,0),vec3(0.5,1,0));f=sf=timer=0;ef=1;aspeed=0.1;
    flags=TRANSPARENT|FACE;
}


void Quad::tick(Transform& trans, float delta)
{
    Transform tr=trans;
//     printf("flags =%d \n",flags);
    if(texture){texture->bind();}
    if(flags&FACE){
        vec3 cp=Camera::active_camera->pos;
        cp=(cp-trans.position).Normalize();
        tr.rotation=quat(vec3(0,0,1),cp*vec3(1,0,1));
    }
    vec3 right=quat::Rotate(tr.rotation,vec3(1,0,0));
    tr.position+=-right*tr.scale.x*0.5;
    tr.update();

    ef=int(1/tile.x*1/tile.y);
    timer+=Input::deltaTime;
    if(timer>aspeed){timer=0;f++;}
    if(f>=ef){f=sf;}
    int tmp=int(1/tile.y);tmp=tmp?tmp:1;    // avoiding % by 0
    float u=f%(tmp);
    float v=int(f/(1/tile.x));
    Shader *shader=Shader::findShader("quad");
    glEnable(GL_DEPTH_TEST);
//     glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    shader->use();
    glUniform2fv(shader->uniTile,1,tile.elements);
    glUniform2fv(shader->uniTileOff,1,vec2(tileoff.x,tileoff.y).elements);
    //glUniform2fv(shader->uniTileOff,1,vec2(u*tile.x,v*tile.y).elements);
    mat4 mvp=Camera::active_camera->proview*tr.model;
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
    glBindVertexArray(Sprite::vao);
    glDrawArrays(GL_TRIANGLES,0,6);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

}

Terrain::Terrain(int sx,int sy,std::ifstream *ifs):sizex(sx),sizey(sy)
{
    scale=4.0;
    texture=0;for(int i=0;i<3;i++){textures[i]=0;}
    float tpx=((sizex-1)*scale)/2.0,tpy=((sizey-1)*scale)/2.0;
    flags=0;type=3;box=Box(vec3(-tpx,0,-tpy),vec3(tpx,0,tpy));
    tile=vec2(1,1);tileoff=vec2(0,0);maxh=-9999;minh=9999;
    raw=new RawMesh();
    raw->vs=sizex*sizey*6;
    raw->vertices=new float[raw->vs];
    if(ifs){
        read(ifs);
    }
    else{
        data=new short[sizex*sizey];
        tdata=new unsigned char[sizex*sizey];
        for(int j=0;j<sizey;j++){
            for(int i=0;i<sizex;i++){
                int v=6*(sizex*j+i);
                raw->vertices[v]=scale*i+(sizex-1)*(-0.5*scale);
                raw->vertices[v+1]=0.0;
                raw->vertices[v+2]=scale*j+(sizey-1)*(-0.5*scale);
            }
        }
    }
    shader=Shader::terrain;
    int numstrips=sizey-1;
    int vperstrip=2*sizex;
    int numdegens=2*(numstrips-1);
    raw->es=numstrips*vperstrip+numdegens;
    raw->indices=new unsigned short[raw->es];
    unsigned int count=0;

    for (int y = 0; y < sizey - 1; y++) {
        if (y > 0) { // from second row
            // Degenerate begin: repeat first vertex
            raw->indices[count++] = (short) (y * sizex);
        }
        for (int x = 0; x < sizex; x++) {
            // One part of the strip
            raw->indices[count++] = (short) ((y * sizex) + x);
            raw->indices[count++] = (short) (((y + 1) * sizex) + x);
        }
        if (y < sizey - 2) { // until last row
            // Degenerate end: repeat last vertex
            raw->indices[count++] = (short) (((y + 1) * sizex) + (sizex - 1));
        }
    }
    raw->vao=0;
    Refresh();
}

void Terrain::Refresh()
{
  reNormals();
  if(raw->vao){
    glDeleteBuffers(1, &raw->ebo);
    glDeleteBuffers(1, &raw->vert_data);
    glDeleteVertexArrays(1, &raw->vao);
  }
  glGenVertexArrays(1, &raw->vao);
  glBindVertexArray(raw->vao);
  glGenBuffers(1, &raw->vert_data);
  glBindBuffer(GL_ARRAY_BUFFER, raw->vert_data);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sizex*sizey*6, raw->vertices, GL_STATIC_DRAW);
  glGenBuffers(1, &raw->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, raw->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned short)*raw->es, raw->indices, GL_STATIC_DRAW);

  //////////////////////////////////////////////////
  ///positions
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
  glEnableVertexAttribArray(normAttrib);
  glVertexAttribPointer(normAttrib,3, GL_FLOAT, GL_FALSE, 6*sizeof(float),(void*)(3*sizeof(float)));
}

void Terrain::tick(Transform& trans, float delta)
{
    shader->use();
    if(texture){texture->bind();}
    glBindVertexArray(raw->vao);
//     glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short)*raw->es, raw->indices);
    mat4 mvp=Camera::active_camera->proview*trans.model;
    glUniformMatrix4fv(shader->uniModel, 1, GL_FALSE, (trans.model.elements));
    mat4 tmat=mat4::Transpose(mat4::Inverse(trans.model));
    glUniformMatrix4fv(glGetUniformLocation(shader->PID,"norMat"), 1, GL_FALSE, (tmat.elements));
    glUniformMatrix4fv(shader->uniMVP, 1, GL_FALSE, (mvp.elements));
    glUniform2fv(shader->uniTile,1,tile.elements);
    glUniform2fv(shader->uniTileOff,1,tileoff.elements);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLE_STRIP,raw->es,GL_UNSIGNED_SHORT,0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
Terrain::~Terrain()
{
  delete raw;delete[] data;delete[] tdata;
}
float * Terrain::terr(unsigned int x, unsigned int y)
{
    return raw->vertices+6*(y*sizex+x);
}
vec3 calcNormal(Terrain*t,unsigned int x,unsigned int z){
    if (x == 0)
      x = 1;
   if (z == 0)
      z = 1;
   float hl = *(t->terr( x - 1, z)+1);
   float hr = *(t->terr( x + 1, z)+1);
   float hd = *(t->terr( x, z + 1)+1); /* Terrain expands towards -Z */
   float hu = *(t->terr( x, z - 1)+1);
   vec3 n = vec3(hl - hr, 2.0f, -hd + hu);
   n.Normalize();
   return n;
}
void Terrain::reNormals()
{
    for(int j=0;j<sizey;j++){
        for(int i=0;i<sizex;i++){
            vec3 n=calcNormal(this,i,j);
            for(int k=0;k<3;k++)
                *(terr(i,j)+3+k)=n[k];
        }
    }
}

btHeightfieldTerrainShape * Terrain::getCollider()
{
    btHeightfieldTerrainShape* htfield=
    new btHeightfieldTerrainShape(sizex,sizey,data,1.0/32.0,-1024.0,1024.0,1,PHY_SHORT,false);
    btVector3 lscale(scale,1.0,scale);
    htfield->setLocalScaling(lscale);

    return htfield;
}
void Terrain::read(std::ifstream* ifs)
{
    ifs->read((char*)&sizex,4);
    ifs->read((char*)&sizey,4);
    ifs->read((char*)&scale,4);
    raw->vs=sizex*sizey*6;
    float tpx=((sizex-1)*scale)/2.0,tpy=((sizey-1)*scale)/2.0;
    box=Box(vec3(-tpx,0,-tpy),vec3(tpx,0,tpy));
    delete []raw->vertices;raw->vertices=new float[raw->vs];
    data=new short[sizex*sizey];
    tdata=new unsigned char[sizex*sizey];
    ifs->read((char*)data,2*sizex*sizey);
    ifs->read((char*)tdata,1*sizex*sizey);
    for(int j=0;j<sizey;j++){
        for(int i=0;i<sizex;i++){
            int v=6*(sizex*j+i);
            raw->vertices[v]=scale*i+(sizex-1)*(-0.5*scale);
            raw->vertices[v+1]=data[j*sizex+i]/32.0;
            raw->vertices[v+2]=scale*j+(sizey-1)*(-0.5*scale);
        }
    }
}
void Terrain::write(std::ofstream& ofs)
{
    ofs.write((char*)&sizex,4);
    ofs.write((char*)&sizey,4);
    ofs.write((char*)&scale,4);
    ofs.write((char*)data,2*sizex*sizey);
    ofs.write((char*)tdata,1*sizex*sizey);

}

btCollisionShape * Mesh::getCollider(char type)
{
    if(type){
        int numtris=int( (float(rawmesh->es) / 3.0));
        btTriangleIndexVertexArray *mi=new btTriangleIndexVertexArray();
        btIndexedMesh me;
        me.m_numTriangles =numtris;
        me.m_triangleIndexBase=(unsigned char*)rawmesh->indices;
        me.m_triangleIndexStride = 3*2;
        me.m_numVertices=rawmesh->vs;
        me.m_vertexBase=(unsigned char*)rawmesh->vertices;
        me.m_vertexStride = 6*4;
        me.m_vertexType = PHY_FLOAT;
        mi->addIndexedMesh(me,PHY_SHORT);
        
        btBvhTriangleMeshShape* tshape=new btBvhTriangleMeshShape(mi,true);
        return tshape;
    }
    else{
    btConvexHullShape* cshape=new btConvexHullShape(rawmesh->vertices,rawmesh->vs,6*4);
    bool approx=false;
    if(approx){
        //create a hull approximation
        btShapeHull* hull = new btShapeHull(cshape);
        btScalar margin = cshape->getMargin();
        hull->buildHull(margin);
        delete cshape;
        cshape=new btConvexHullShape(0,0,0);
        const btVector3* tverts=hull->getVertexPointer();
        for(int i=0;i<hull->numVertices()-1;i++){
            cshape->addPoint(tverts[i],false);
        }
        cshape->addPoint(tverts[hull->numVertices()-1],true);
        delete hull;
    }
    return cshape;
    }
}

