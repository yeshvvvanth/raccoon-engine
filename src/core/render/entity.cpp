#include"entity.h"
#include"../utils/data.h"
#include"../main/scene.h"
btDiscreteDynamicsWorld* Entity::dynamicsWorld=0;

#define IMESH       1
#define IPHYSICS    2
#define ISCRIPT     4
#define ICAMERA     8
#define IQUAD       16
#define ITERRAIN    32
#define ILIGHT      64
#define ISOUND      128
#define IWAYPOINT   256
#define IGRIDMAP    512
#define IBUTTON     1024
#define IPARTICLES  2048
#define IJOINT      4096
#define ISPLINE     8192
#define IDECAL
#define IFRAMEBUFFER

//13

Entity::Entity(std::string nam):name(nam),camera(0),light(0),sound(0)
{
 mesh=0;parent=0;renderFlags=NOSHADOW;button=0;enabled=true;
 rigidBody=0;collShape=0;script=0;waypoint=0;gridmap=0;
 
}

void Entity::remove(Entity* dent)
{
    std::vector<Entity*> tchildren=dent->children;
    int ind=-1,i=0;
    for(i=0;i<tchildren.size();i++){
        remove(tchildren[i]);
    }
    dent->unparent();
    std::vector<Entity*>& ents=Scene::activeScene->entities;
    for(i=0;i<ents.size();i++){
        if(dent==ents[i]){ind=i;break;}
    }
    if(ind>-1){
        ents.erase(ents.begin()+ind);
        delete dent;
    }
    else{
        printf("entity not found!!\n\n");
        printf("name of scene=%s \n",Scene::activeScene->name.c_str());
    }
}

void Entity::removeComp(short type)
{
    if(type==1 && rigidBody){//physics 
        if(rigidBody->isInWorld())
            dynamicsWorld->removeRigidBody(rigidBody);
        if(!source.length()){
            if(collShape->getShapeType()==TRIANGLE_MESH_SHAPE_PROXYTYPE){
                delete ((btBvhTriangleMeshShape*)collShape)->getMeshInterface();
            }
            delete collShape;
        }
        delete rigidBody;
        delete motionState;
        rigidBody=0;
    }
    else if(type==0&&mesh){
        delete mesh;mesh=0;
    }
    else if(type==2&&light){//light
        delete light;light=0;
    }
    else if(type==3){
        
    }
}


Entity::~Entity()                   
{
  if(script){delete script;}
  if(sound)delete sound;
  if(camera){
      if(Scene::activeScene&&Scene::activeScene->camera==camera)
        {Scene::activeScene->camera=0;}
      delete camera;
  }
  if(button){delete button;}
  if(mesh)delete mesh;
  if(rigidBody){
     removeComp(1); // delete rigidBody
  }
  if(waypoint)delete waypoint;
  if(light)delete light;
  
}

void Entity::init()
{
    if(script){
            script->Init();
    }
    if (sound&&sound->flags&PLAYONAWAKE&&!sound->playing){
        sound->Play();
    }

    
}

void Entity::render()
{
    if(isEnabled()&&mesh && renderFlags^NORENDER){
        mesh->box.tick(rtrans);
        if(mesh->type==1){
            ((Mesh*)mesh)->rawmesh->shader->use();
            if(mesh->flags&TRANSPARENT){glEnable(GL_BLEND);;}
            mat4 mvp=Camera::active_camera->proview*rtrans.model;
            glUniformMatrix4fv(((Mesh*)mesh)->rawmesh->shader->uniMVP, 1, GL_FALSE, (mvp.elements));
    //         glUniform1f(mesh->shader->uniSpec,0.3);
            
            glUniform1f(((Mesh*)mesh)->rawmesh->shader->uniSmooth,((Mesh*)mesh)->smoothness+0.001);
   
            glUniform1i(glGetUniformLocation(((Mesh*)mesh)->rawmesh->shader->PID,"doShadow"),renderFlags^NOSHADOW);
            
            Light::reset(((Mesh*)mesh)->rawmesh->shader);
            std::vector<Light*> lights,tl;std::vector<float> dists;
            for(int k=0;k<Scene::activeScene->entities.size();k++){
                
                Entity* le=Scene::activeScene->entities[k];
                if( le->light ){
                    if(le->light->type==0 ){
                        le->light->update(((Mesh*)mesh)->rawmesh->shader,0);
                    }
                    else if(le->light->type==1 ){
                        Light* pl=(le->light);float dist;
                        if(le->enabled && mesh->box.IntersectSphere(pl->position,pl->radius*pl->radius,dist)){
                            dist+=2*(transform.position.SqDistance(pl->position));
                            lights.push_back(pl);dists.push_back(dist);
                        }
                    }
                }
            }
            if(lights.size()>NUM_PLIGHTS)
            {
                for(int j=0;j<(lights.size());j++){
                    short lt=0;
                    for(int k=0;k<(lights.size());k++){
                        if(dists[j]>dists[k]){lt++;}
                    }
                    if(lt<4){tl.push_back(lights[j]);}
                }
                lights=tl;
            }
            for(int k=0;k<lights.size();k++)
            {
                lights[k]->update(((Mesh*)mesh)->rawmesh->shader);
            }
            
            mesh->tick(rtrans);
            ((Mesh*)mesh)->rawmesh->render(rtrans);
        }
        else if(mesh->type==2){
            
            mesh->tick(rtrans,Input::deltaTime);
        }
        else if(mesh->type==3){
            
//             glUniform1i(glGetUniformLocation(((Terrain*)mesh)->shader->PID,"doShadow"),1);
            Light::reset(((Terrain*)mesh)->shader);
            std::vector<Light*> lights,tl;std::vector<float> dists;
            for(int k=0;k<Scene::activeScene->entities.size();k++){
                
                Entity* le=Scene::activeScene->entities[k];
                if( le->light ){
                    if(le->light->type==0 ){
                        le->light->update(((Terrain*)mesh)->shader,0);
                    }
                    else if(le->light->type==1 ){
                        Light* pl=(le->light);float dist;
                        if(le->enabled && mesh->box.IntersectSphere(pl->position,pl->radius*pl->radius,dist)){
                            dist+=2*(transform.position.SqDistance(pl->position));
                            lights.push_back(pl);dists.push_back(dist);
                        }
                    }
                }
            }
            if(lights.size()>NUM_PLIGHTS)
            {
                for(int j=0;j<(lights.size());j++){
                    short lt=0;
                    for(int k=0;k<(lights.size());k++){
                        if(dists[j]>dists[k]){lt++;}
                    }
                    if(lt<4){tl.push_back(lights[j]);}
                }
                lights=tl;
            }
            for(int k=0;k<lights.size();k++)
            {
                lights[k]->update(((Terrain*)mesh)->shader);
            }
            
            mesh->tick(rtrans,Input::deltaTime);
        }
        glDisable(GL_BLEND);
    }
}
void Entity::tick()
{
    if(enabled){
        if(light){
            // check it !
            light->position=rtrans.position;
            light->direction=quat::Rotate(rtrans.rotation,vec3(0.0,0.0,1.0));
        }
        if(sound){sound->tick(transform.position);}
    }
    if(isEnabled()&&rigidBody){
        // if kinematic
        if((rigidBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)){
            btTransform et(toBQuat(rtrans.rotation),toBVec3(rtrans.position));
            rigidBody->setCenterOfMassTransform(et);
        }
        else{// not kinematic 
            btTransform trans;
            
            Transform partrans,nptrans;
            {
//                 trans=motionState->m_graphicsWorldTrans;
                rigidBody->getMotionState()->getWorldTransform(trans);
                trans=trans*motionState->m_centerOfMassOffset; 
//                 trans=rigidBody->getWorldTransform(); ///never use that 
                if(parent)partrans=parent->rtrans;
            }
            
                vec3 pos=toVec3(trans.getOrigin());
                quat rot=toQuat(trans.getRotation());
                transform.position=pos;
                
                nptrans.position=pos;nptrans.rotation=rot;nptrans.scale=rtrans.scale;
                partrans.update();nptrans.update();
                
                transform.rotation=partrans.rotation.inverse()*rot;
                
                mat4 mat=mat4::Inverse(partrans.model)*nptrans.model;
                transform.position=vec3(mat.elements[12],mat.elements[13],mat.elements[14]);
                
            transform.update();
        }
        
    }

}
void Entity::update()
{
//     if(mesh&&mesh->type==3){rtrans=Transform();return;}
    rtrans=transform;
    ptrans=Transform();
    transform.update();
    Entity *par=parent;
    while(par!=0){
        ptrans.multiply( par->transform);
        par=par->parent;
    }
    rtrans.multiply(ptrans);
}



void Entity::read(std::ifstream& ifs,Scene* scene)
{
  ifs.read((char*)transform.scale.elements,3*sizeof(float));
  ifs.read((char*)transform.rotation.elements,4*sizeof(float));
  ifs.read((char*)transform.position.elements,3*sizeof(float));
  transform.update();rtrans=transform;
  
  unsigned char namlen=0;
  ifs.read((char*)&namlen,1);
  char *entnam=new char[namlen+1];entnam[namlen]='\0';
  ifs.read((char*)entnam,namlen);
  name=std::string(entnam);
  delete []entnam;
  
/// reading hierarchy ///
  unsigned short parID=0,ch_num,*childIDS=0;
  ifs.read((char*)&enabled,1);
  ifs.read((char*)&parID,sizeof(short));

  ifs.read((char*)&ch_num,sizeof(short));
  childIDS=new unsigned short [ch_num];
  ifs.read((char*)childIDS,ch_num*sizeof(short));
  
  if(parID!=NONE)
      this->parent=scene->entities[parID];
 
  for(int k=0;k<ch_num;k++){
      if(childIDS[k]!=NONE)this->children.push_back(scene->entities[childIDS[k]]);
  }
   delete[] childIDS;
  // reading if this is an instanced entity
  ifs.read((char*)&namlen,1);
  if(namlen){
      char *entnam=new char[namlen+1];entnam[namlen]='\0';
      ifs.read((char*)entnam,namlen);
      source=std::string(entnam);
      delete []entnam;
      return;
  }
  int contents=0;
  ifs.read((char*)&contents,sizeof(int));
   /// reading ass names /////////////
  unsigned char tnlen=0,snlen=0,anlen=0;
  char *mname=0,*tname=0,*aname=0,*sname=0;
  
  // Reading RawMesh
  if(contents&IMESH||contents&IQUAD||contents&ITERRAIN){
     if(contents&IMESH){ // Mesh
        unsigned char mnlen=0;
        ifs.read((char*)&mnlen,1);
        ifs.read((char*)&mnlen,1);
        mname=new  char[mnlen+1];mname[mnlen]='\0';
        short manum;float msmoothness;
        ifs.read((char*)mname,mnlen);
        ifs.read((char*)&manum,2);
        ifs.read((char*)&msmoothness,4);
        RawMesh* rm=RawMesh::findRawMesh(mname,0);
        if(rm)mesh=new Mesh(rm);
        delete []mname;
        if(mesh){((Mesh*)mesh)->smoothness=msmoothness;((Mesh*)mesh)->anum=manum;}
    }
    else if(contents&IQUAD){  // quad
        mesh=new Quad();
        ifs.read((char*)&((Quad*)mesh)->sf,2);
        ifs.read((char*)&((Quad*)mesh)->ef,2);
        ifs.read((char*)&((Quad*)mesh)->aspeed,4);
    }
    else if(contents&ITERRAIN){ // terrain
        mesh=new Terrain(128,128,&ifs);
    }
    vec2 ttile,ttileoff;char mflags;
    ifs.read((char*)&mflags,1);
    ifs.read((char*)ttile.elements,4*2);
    ifs.read((char*)ttileoff.elements,4*2);
    ifs.read((char*)&tnlen,1);
    if(mesh){mesh->tile=ttile;mesh->tileoff=ttileoff;mesh->flags=mflags;}
    if(tnlen){
        tname=new  char[tnlen+1];tname[tnlen]='\0';
        ifs.read((char*)tname,tnlen);
        if(mesh)mesh->texture=Texture::findTexture(tname,false);
        delete[] tname;
        for(int k=0;mesh&&mesh->type==3&&k<3;k++){
            ifs.read((char*)&tnlen,1);
            tname=new  char[tnlen+1];tname[tnlen]='\0';
            ifs.read((char*)tname,tnlen);
            ((Terrain*)mesh)->textures[k]=Texture::findTexture(tname,false);
            delete[] tname;
        }
    }
  }
  // read SOUND
  if(contents&ISOUND){
      ifs.read((char*)&anlen,1);
      aname=new char[anlen+1];aname[anlen]='\0';
      ifs.read((char*)aname,anlen);
      
      Audio* audio=Audio::findAudio(aname);
      char sflags;float gain,pitch;
      
       ifs.read((char*)&sflags,1);
       ifs.read((char*)&gain,4);
       ifs.read((char*)&pitch,4);
      if(audio){
          sound=new Sound(audio);
          sound->flags=sflags;
          sound->gain=gain;
          sound->pitch=pitch;
      }
       
      delete []aname;
  }
  
  ///////// read SCRIPTS ////////////
  if(contents&ISCRIPT){  
    ifs.read((char*)&snlen,1);
    sname=new  char[snlen+1];sname[snlen]='\0';
    ifs.read((char*)sname,snlen);
    
    for(int j=0;j<Script::scripts.size();j++){
        if(strcmp(sname,Script::scripts[j]->name.c_str())==0){
            script=Script::scripts[j]->clone();script->entity=this;break;
        }
    }
    delete[] sname;
    unsigned char numProps=0;
    ifs.read((char*)&numProps,1);
    for(short q=0;q<numProps;q++){
        ScriptProp *sp=0;
        char* tname;unsigned char stype=0;
        bool b=0;float f=0;
        
        ifs.read((char*)&snlen,1);
        tname=new char[snlen+1]; tname[snlen]='\0';
        ifs.read((char*)tname,snlen);
        // if script exists, find prop
        if(script)sp=script->findProp(tname);
        delete []tname;
        ifs.read((char*)&stype,1);
        if(stype==1){
            ifs.read((char*)&f,4);
            if(sp)((SFloat*)sp)->value=f;
        }else if(stype==2){
            ifs.read((char*)&b,1);
            if(sp)((Sbool*)sp)->value=b;
        }else if(stype==3){
            ifs.read((char*)&snlen,1);
            tname=new char[snlen+1];tname[snlen]='\0';
            ifs.read((char*)tname,snlen);
            if(sp)((Sstring*)sp)->value=std::string(tname);
            delete []tname;
        }
    }
  }
   
  // camera 
  if(contents&ICAMERA){
      camera=new Camera();
      ifs.read((char*)&camera->ortho,1);
      ifs.read((char*)&camera->aspect_ratio,4);
      ifs.read((char*)&camera->near,4);
      ifs.read((char*)&camera->far,4);
      if(camera->ortho)
        ifs.read((char*)&camera->size,4);
      else
        ifs.read((char*)&camera->fov,4);
      camera->set();
  }
  
  /// lights 
    if(contents&ILIGHT){
        unsigned char ltype=0;
        ifs.read((char*)&ltype,1);
        if(ltype==T_DIRECT)
            light=new Light(T_DIRECT);
        else if(ltype==T_POINT)
                light=new Light(T_POINT);
        else if(ltype==T_SPOTLIGHT)
                    light=new Light(T_SPOTLIGHT);
        
        ifs.read((char*)&light->intensity,4);
        ifs.read((char*)light->colour.elements,4*3);
        ifs.read((char*)&light->shadow,1);
        if(light->type==T_POINT)
            ifs.read((char*)&(light)->radius,4);
        if(light->type==T_SPOTLIGHT)
            ifs.read((char*)&(light)->angle,4);
    }
  // read physics 
    if(contents&IPHYSICS){
        unsigned char tcp=0;
        ifs.read((char*)&tcp,1);
        // read offset
        vec3 offs,afac,lifac;quat roffs;float mass=0;
        char cflags=0;
        float friction,restitution;
        ifs.read((char*)&mass,4);
        ifs.read((char*)&friction,4);
        ifs.read((char*)&restitution,4);
        ifs.read((char*)&cflags,1);
        ifs.read((char*)afac.elements,4*3);
        ifs.read((char*)lifac.elements,4*3);
        ifs.read((char*)offs.elements,sizeof(float)*3);
        ifs.read((char*)roffs.elements,sizeof(float)*4);
        // read transform 
        quat &rot=transform.rotation;vec3 & pos=transform.position;
        motionState= new MotionState(btTransform(toBQuat(rot),toBVec3(pos)),
                            btTransform(toBQuat(roffs),toBVec3(offs)));
        bool safe=true;
        if (tcp==SPHERE_SHAPE_PROXYTYPE){
            float radius=0;
            ifs.read((char*)&radius,4);
            collShape=new btSphereShape(radius);
        }
        else if (tcp==BOX_SHAPE_PROXYTYPE){
            vec3 hal;
            ifs.read((char*)hal.elements,4*3);
            collShape=new btBoxShape(toBVec3(hal));
        }
        else if (tcp==CYLINDER_SHAPE_PROXYTYPE){
            vec3 hal;
            ifs.read((char*)hal.elements,4*3);
            collShape=new btCylinderShape(toBVec3(hal));
        }
        else if (tcp==CAPSULE_SHAPE_PROXYTYPE){
            float radius=0;
            float halfheight=0;
            ifs.read((char*)&radius,4);
            ifs.read((char*)&halfheight,4);
            collShape=new btCapsuleShape(radius,halfheight*2);
        }
        else if (tcp==TERRAIN_SHAPE_PROXYTYPE&&mesh&&mesh->type==3){
            collShape=((Terrain*)mesh)->getCollider();
        }
        else if (tcp==STATIC_PLANE_PROXYTYPE){
            vec3 nor;
            float dist=0;
            ifs.read((char*)nor.elements,4*3);
            ifs.read((char*)&dist,4);
            collShape=new btStaticPlaneShape(toBVec3(nor),0);
        }
        else if (tcp==CONVEX_HULL_SHAPE_PROXYTYPE&&mesh&&mesh->type==1){
            collShape=((Mesh*)mesh)->getCollider();
            collShape->setLocalScaling(toBVec3(rtrans.scale));
        }
        else if (tcp==TRIANGLE_MESH_SHAPE_PROXYTYPE&&mesh&&mesh->type==1){
            collShape=((Mesh*)mesh)->getCollider(1);
            collShape->setLocalScaling(toBVec3(rtrans.scale));
        }
        else{
            delete motionState;motionState=0;safe=false;
        }
        if(safe){
            btVector3 inertia(0.0,0.0,0.0);
            collShape->calculateLocalInertia(mass,inertia);
            rigidBody=new btRigidBody(mass,motionState,collShape,inertia);
//             dynamicsWorld->addRigidBody(rigidBody);
                
            rigidBody->setFriction(friction);
            rigidBody->setRestitution(restitution);
            rigidBody->setAngularFactor(toBVec3(afac));
            rigidBody->setLinearFactor(toBVec3(lifac));
            rigidBody->setCollisionFlags(cflags);
            if(tcp==TERRAIN_SHAPE_PROXYTYPE)
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        }
    }
    // read UI thingies
    vec3 bp,bs;float fscale=Label::dfsize;
    if(contents&IBUTTON){
        char btype=0;
        ifs.read((char*)&btype,1);
        ifs.read((char*)&bp.x,4);ifs.read((char*)&bp.y,4);
        ifs.read((char*)&bs.x,4);ifs.read((char*)&bs.y,4);
        short nlen=0;
        ifs.read((char*)&nlen,1);
        Texture *bt=0;
        if(nlen>0){
            char *nam=0;
            nam=new char[nlen+1];nam[nlen]='\0';
            ifs.read((char*)nam,nlen);
            bt=Texture::findTexture(nam,0);
            delete []nam;
        }
        if(btype==0||btype==6){
            button=new Button(bp.x,bp.y,bs.x,bs.y);
            button->type=btype;
        }
        else if(btype==1 || btype==5){
            char *nam=0;short nlen=0;
            ifs.read((char*)&nlen,1);
            nam=new char[nlen+1];nam[nlen]='\0';
            ifs.read((char*)nam,nlen);
            ifs.read((char*)&fscale,4);
            TextButton* tb;
            if(btype==5){tb=new TextButton(bp.x,bp.y,nam,0,true);}
            else{tb=new TextButton(bp.x,bp.y,nam,0);}
            ifs.read((char*)((TextButton*)tb)->label->color.elements,4*3);
            //font name
            ifs.read((char*)&nlen,1);
            if(nlen){
                delete []nam;
                nam=new char[nlen+1];nam[nlen]='\0';
                ifs.read((char*)nam,nlen);
                Font* tff=Font::findFont(std::string(nam));
                if(tff){tb->label->font=tff;tb->label->setText(tb->label->text);}
            }
            if(tb->type==1){
                ifs.read((char*)&tb->padX,4);
                ifs.read((char*)&tb->padY,4);
            }
            tb->label->setScale(fscale);
            tb->setSize(bs.x,bs.y);
            button=tb;
            delete []nam;
        }
        else if(btype==2){
            button=new Slider(bp.x,bp.y,bs.x,bs.y);
            ifs.read((char*)&nlen,1);
            if(nlen>0){
                char *nam=0;
                nam=new char[nlen+1];nam[nlen]='\0';
                ifs.read((char*)nam,nlen);
                ((Slider*)button)->handle->texture=Texture::findTexture(nam,0);
                delete []nam;
            }
            ifs.read((char*)&((Slider*)button)->value,4);
        }
        if(button->type!=5&&button->type!=2){
            ifs.read((char*)button->uColor.elements,4*3);
            ifs.read((char*)&button->alpha,4);
        }
        button->texture=bt;
        if(button->type==6)
        button->transform.rotation=transform.rotation;
    }
    //read gridmap
    if(contents&IGRIDMAP){
        unsigned short sx=8,sy=8;
        ifs.read((char*)&sx,2);
        ifs.read((char*)&sy,2);
        gridmap=new GridMap(sx,sy);
        ifs.read((char*)gridmap->dimensions.elements,4*3);
        ifs.read((char*)gridmap->tiles,sx*sy*2*sizeof(unsigned char));
        unsigned short nums=0;
        ifs.read((char*)&nums,2);
        for(int i=0;i<nums;i++){
                unsigned char nlen=0;
                ifs.read((char*)&nlen,1);
                char* tsname=new char[nlen+1];tsname[nlen]='\0';
                ifs.read((char*)tsname,nlen);
                gridmap->snames.push_back(std::string(tsname));
                delete []tsname;
        }
        /*
        */

    }
        //read waypoint
    if(contents&IWAYPOINT){
        waypoint=new Waypoint();
    }
  
    // end of read
 
}
void Entity::write(std::ofstream& ofs,Scene* scene)
{
  ofs.write((char*)transform.scale.elements,3*sizeof(float));
  ofs.write((char*)transform.rotation.elements,4*sizeof(float));
  ofs.write((char*)transform.position.elements,3*sizeof(float));
  unsigned char namlen=name.size();
  ofs.write((char*)&namlen,1);
  ofs.write((char*)name.c_str(),namlen);

  /// writing hierarchy ///
  unsigned short u=0,v=0,ch_num=children.size();
  unsigned short parID=NONE,*childIDS=new unsigned short[ch_num];
  for(int i=0;i<ch_num;i++){childIDS[i]=NONE;}
  if(parent){
    for(u=0;u<scene->entities.size();u++){
        if(parent==scene->entities[u]){parID=u;break;}
    }
  }
  if(!gridmap){
    for(u=0;u<children.size();u++){
        for(v=0;v<scene->entities.size();v++){
        if(children[u]==scene->entities[v]){childIDS[u]=v;break;}
        }
    }
  }
  else{
    ch_num=0;
  }
  ofs.write((char*)&enabled,1);
  ofs.write((char*)&parID,sizeof(short));
  ofs.write((char*)&ch_num,sizeof(short));
  ofs.write((char*)childIDS,ch_num*sizeof(short));
  delete[] childIDS;
  //writing the source of this entity instance if it is one
  namlen=source.length();
  ofs.write((char*)&namlen,1);
  if(namlen){
       ofs.write((char*)source.c_str(),namlen);
      return;
  }
  int contents=0;
  if(mesh){
      if(mesh->type==1)contents|=IMESH;
      else if(mesh->type==2)contents|=IQUAD;
      else if(mesh->type==3)contents|=ITERRAIN;
  }
  if(light)contents|=ILIGHT;
  if(sound)contents|=ISOUND;
  if(rigidBody)contents|=IPHYSICS;
  if(camera)contents|=ICAMERA;
  if(waypoint)contents|=IWAYPOINT;
  if(script)contents|=ISCRIPT;
  if(gridmap)contents|=IGRIDMAP;
  if(button)contents|=IBUTTON;
  if(light)contents|=ILIGHT;
  /// saving ass names /////////
  unsigned char mnlen=0,tnlen=0,anlen,snlen=0;
  unsigned short p=0;
  
  ofs.write((char*)&contents,sizeof(int));  
  
  mnlen= mesh==0?0:mesh->type;
  tnlen= (mesh&&mesh->texture) ? strlen(mesh->texture->name.c_str()):0;
  anlen= sound!=0 ? strlen(sound->audio->name.c_str()):0;
  printf("saving mesh \n");

  if(mesh){
    if( mesh->type==2){// Quad
        ofs.write((char*)&((Quad*)mesh)->sf,2);
        ofs.write((char*)&((Quad*)mesh)->ef,2);
        ofs.write((char*)&((Quad*)mesh)->aspeed,4);
    }
    else if(mesh->type==3){// Terrain
        ((Terrain*)mesh)->write(ofs);
    }
    else if(mesh->type==1){
        ofs.write((char*)&mnlen,1); 
        mnlen=((Mesh*)mesh)->rawmesh->name.length();
        ofs.write((char*)&mnlen,1);
        ofs.write((char*)((Mesh*)mesh)->rawmesh->name.c_str(),mnlen);
        // texture

        ofs.write((char*)&((Mesh*)mesh)->anum,2);
        ofs.write((char*)&((Mesh*)mesh)->smoothness,4);
    }
    ofs.write((char*)&mesh->flags,1);
    ofs.write((char*)mesh->tile.elements,4*2);
    ofs.write((char*)mesh->tileoff.elements,4*2);
    ofs.write((char*)&tnlen,1);
    if(tnlen){
        ofs.write((char*)mesh->texture->name.c_str(),tnlen);
        for(int k=0;mesh->type==3&&k<3;k++){
            Texture* te=((Terrain*)mesh)->textures[k];
            tnlen=te!=0?te->name.length():0;
            ofs.write((char*)&tnlen,1);
            if(tnlen)
                ofs.write((char*)te->name.c_str(),tnlen);
        }
    }
    // 
  }
  // sound
  if(sound){
      ofs.write((char*)&anlen,1);
      ofs.write((char*)sound->audio->name.c_str(),anlen);
     
       ofs.write((char*)&sound->flags,1);
       ofs.write((char*)&sound->gain,4);
       ofs.write((char*)&sound->pitch,4);
  }
  if(script){
    snlen=script->name.length();
    ofs.write((char*)&snlen,1);
    ofs.write((char*)script->name.c_str(),snlen);
    unsigned char numProps=script->props.size();
    ofs.write((char*)&numProps,1);
    for(short q=0;q<numProps;q++){
        ScriptProp *sp=script->props[q];
        snlen=sp->name.length();
        ofs.write((char*)&snlen,1);
        ofs.write((char*)sp->name.c_str(),snlen);
        ofs.write((char*)&sp->type,1);
        if(sp->type==1)
            ofs.write((char*)&((SFloat*)sp)->value,4);
        else if(sp->type==2)
            ofs.write((char*)&((Sbool*)sp)->value,1);
        else if(sp->type==3){
            snlen=((Sstring*)sp)->value.length();
            ofs.write((char*)&snlen,1);
            ofs.write((char*)((Sstring*)sp)->value.c_str(),snlen);
        }
    }
  }
  //save camera
  if(camera){
      ofs.write((char*)&camera->ortho,1);
      ofs.write((char*)&camera->aspect_ratio,4);
      ofs.write((char*)&camera->near,4);
      ofs.write((char*)&camera->far,4);
      if(camera->ortho)
        ofs.write((char*)&camera->size,4);
      else
        ofs.write((char*)&camera->fov,4);
  }
  //save lights
  if(light){
      ofs.write((char*)&light->type,1);
      ofs.write((char*)&light->intensity,4);
      ofs.write((char*)light->colour.elements,4*3);
      ofs.write((char*)&light->shadow,1);
      if(light->type==T_POINT)
          ofs.write((char*)&(light)->radius,4);
      if(light->type==T_SPOTLIGHT)
          ofs.write((char*)&(light)->angle,4);
  }
  // save physics things
  if(rigidBody){
        char tcp=rigidBody->getCollisionShape()->getShapeType();
        ofs.write((char*)&tcp,1);
        // save offset
        float mass=1/rigidBody->getInvMass(),friction=rigidBody->getFriction(),
        restitution=rigidBody->getRestitution();
        vec3 offs=toVec3(motionState->m_centerOfMassOffset.getOrigin());
        quat roffs=toQuat(motionState->m_centerOfMassOffset.getRotation());
        vec3 lifac=toVec3(rigidBody->getLinearFactor());
        vec3 afac=toVec3( rigidBody->getAngularFactor());
        char cflags=rigidBody->getCollisionFlags();
        
        ofs.write((char*)&mass,4);
        ofs.write((char*)&friction,4);
        ofs.write((char*)&restitution,4);
        ofs.write((char*)&cflags,1);
        
        ofs.write((char*)afac.elements,4*3);
        ofs.write((char*)lifac.elements,4*3);
        
        ofs.write((char*)offs.elements,sizeof(float)*3);
        ofs.write((char*)roffs.elements,sizeof(float)*4);
        
        if (tcp==SPHERE_SHAPE_PROXYTYPE){
            float radius=((btSphereShape*)collShape)->getRadius();
            ofs.write((char*)&radius,4);
        }
        else if (tcp==BOX_SHAPE_PROXYTYPE){
            btVector3 halfs=((btBoxShape*)collShape)->getHalfExtentsWithMargin();
            vec3 hal(halfs.x(),halfs.y(),halfs.z());
            ofs.write((char*)hal.elements,4*3);
        }
        else if (tcp==CYLINDER_SHAPE_PROXYTYPE){
            btVector3 halfs=((btCylinderShape*)collShape)->getHalfExtentsWithMargin();
            vec3 hal(halfs.x(),halfs.y(),halfs.z());
            ofs.write((char*)hal.elements,4*3);
        }
        else if (tcp==CAPSULE_SHAPE_PROXYTYPE){
            float radius=((btCapsuleShape*)collShape)->getRadius();
            float halfheight=((btCapsuleShape*)collShape)->getHalfHeight();
            ofs.write((char*)&radius,4);
            ofs.write((char*)&halfheight,4);
        }
        else if (tcp==TERRAIN_SHAPE_PROXYTYPE){
            
        }
        else if (tcp==STATIC_PLANE_PROXYTYPE){
            btVector3 norm=((btStaticPlaneShape*)collShape)->getPlaneNormal();
            vec3 nor(norm.x(),norm.y(),norm.z());
            float dist=((btStaticPlaneShape*)collShape)->getPlaneConstant();
            ofs.write((char*)nor.elements,4*3);
            ofs.write((char*)&dist,4);
        }
        else if (tcp==CONVEX_HULL_SHAPE_PROXYTYPE){
            
        }
  }

  // save UI thingies
  if(button){
      vec3 bp=button->transform.position,bs=button->transform.scale;
        ofs.write((char*)&button->type,1);
        ofs.write((char*)&bp.x,4);ofs.write((char*)&bp.y,4);
        ofs.write((char*)&bs.x,4);ofs.write((char*)&bs.y,4);
        if(button->texture){
            char nlen=button->texture->name.length();
            ofs.write((char*)&nlen,1);
            ofs.write((char*)button->texture->name.c_str(),nlen);
        }
        else{char nlen=0;ofs.write((char*)&nlen,1);}
        if(button->type==1 || button->type==3 || button->type==5){
            TextButton* tb=((TextButton*)button);
            char nlen=tb->label->text.length();
            float fscale=tb->label->scale;
            ofs.write((char*)&nlen,1);
            ofs.write((char*)tb->label->text.c_str(),nlen);
            ofs.write((char*)&fscale,4);
            ofs.write((char*)tb->label->color.elements,4*3);
            nlen=0;
            if(tb->label->font!=Font::efonts[0]){
                nlen=tb->label->font->name.length();
            }
            ofs.write((char*)&nlen,1);
            if(nlen)ofs.write((char*)tb->label->font->name.c_str(),nlen);
            if(button->type==1){
                ofs.write((char*)&((TextButton*)button)->padX,4);
                ofs.write((char*)&((TextButton*)button)->padY,4);
            }
        }
        else   if(button->type==2){
            if(((Slider*)button)->handle->texture){
                char nlen=((Slider*)button)->handle->texture->name.length();
                ofs.write((char*)&nlen,1);
                ofs.write((char*)((Slider*)button)->handle->texture->name.c_str(),nlen);
            }
            else{char nlen=0;ofs.write((char*)&nlen,1);}
            ofs.write((char*)&((Slider*)button)->value,4);
        }
        if(button->type!=5&&button->type!=2){
            ofs.write((char*)button->uColor.elements,4*3);
            ofs.write((char*)&button->alpha,4);
        }
      
  }
    
  //save gridmap
  if(gridmap){
     unsigned short sx=gridmap->sizex,sy=gridmap->sizey;
     ofs.write((char*)&sx,2);
     ofs.write((char*)&sy,2);
     ofs.write((char*)gridmap->dimensions.elements,4*3);
     ofs.write((char*)gridmap->tiles,sx*sy*2*sizeof(unsigned char));
     unsigned short nums=gridmap->snames.size();
     ofs.write((char*)&nums,2);
     for(int i=0;i<nums;i++){
            unsigned char nlen=gridmap->snames[i].length();
            ofs.write((char*)&nlen,1);
            ofs.write((char*)gridmap->snames[i].c_str(),nlen);
     }
  }
  
    //save waypoint
  if(waypoint){
      
  }  
  /// end ///
}

Entity* Entity::clone(bool par,bool inst){
    Entity* tent=new Entity(name);
    *tent=*this;
    tent->source=source;
    if(waypoint){
        tent->waypoint=new Waypoint();
        Scene::activeScene->bakeNavigation();
    }
    if(gridmap){
        tent->gridmap=new GridMap();
    }
    if(camera){
        tent->camera=new Camera(*(camera));
        if(!Scene::activeScene->camera)Scene::activeScene->camera=tent->camera;
    }
    if(script){
        tent->script=script->clone();
        tent->script->entity=tent;
    }
    if(mesh){
        if(mesh->type==1){
            tent->mesh=new Mesh(* ((Mesh*)mesh) );
        }
        else if(mesh->type==2){// Quad render
            tent->mesh=new Quad(* ((Quad*)mesh) );
        }
        else if(mesh->type==3){// Terrain
//             Terrain* ter=((Terrain*)mesh);
//             tent->mesh=new Terrain(ter->posx,ter->posy);
        }
    }
    if(button){
        if(button->type==0||button->type==6){
            Transform& tr=button->transform;
            tent->button=new Button(tr.position.x,tr.position.y,tr.scale.x,tr.scale.y);
            tent->button->type=button->type;
        }
        else if(button->type==1||button->type==5){
            if(button->type==5)
                tent->button=new TextButton(0,0,((TextButton*)button)->label->text,0,true);
            else
                tent->button=new TextButton(0,0,((TextButton*)button)->label->text);
            tent->button->transform=button->transform;
            ((TextButton*)tent->button)->padX=((TextButton*)button)->padX;
            ((TextButton*)tent->button)->padY=((TextButton*)button)->padY;
            *((TextButton*)tent->button)->label=*(((TextButton*)button)->label);
        }
        else if(button->type==2){
            Transform& tr=button->transform;
            tent->button=new Slider(tr.position.x,tr.position.y,tr.scale.x,tr.scale.y);
            ((Slider*)tent->button)->value=((Slider*)button)->value;
        }
        tent->button->texture=button->texture;
    }
    if(light){
        tent->light=new Light(*light);
    }
    if(sound){
        tent->sound=new Sound(sound->audio);
        tent->sound->flags=sound->flags;
        tent->sound->gain=sound->gain;
        tent->sound->pitch=sound->pitch;
    }
    if(rigidBody){
        // clone the physics thingies
        tent->motionState=new MotionState(*motionState);
        tent->collShape=collShape;
        if(!inst){
            if(collShape->getShapeType()==BOX_SHAPE_PROXYTYPE)
            {tent->collShape=new btBoxShape(*((btBoxShape*)collShape));}
            else if(collShape->getShapeType()==SPHERE_SHAPE_PROXYTYPE)
            {tent->collShape=new btSphereShape(*((btSphereShape*)collShape));}
            else if(collShape->getShapeType()==CAPSULE_SHAPE_PROXYTYPE){
                tent->collShape=new btCapsuleShape(*((btCapsuleShape*)collShape));
            }
            else if(collShape->getShapeType()==CYLINDER_SHAPE_PROXYTYPE){
                tent->collShape=new btCylinderShape(*((btCylinderShape*)collShape) );
            }
            else if(collShape->getShapeType()==STATIC_PLANE_PROXYTYPE){
                tent->collShape=new btStaticPlaneShape(*((btStaticPlaneShape*)collShape));
            }
            else if(collShape->getShapeType()==TERRAIN_SHAPE_PROXYTYPE&&mesh&&mesh->type==3)
            {tent->collShape=((Terrain*)mesh)->getCollider();}
            else if(collShape->getShapeType()==CONVEX_HULL_SHAPE_PROXYTYPE&&mesh&&mesh->type==1)
            {tent->collShape=((Mesh*)mesh)->getCollider();tent->collShape->setLocalScaling(toBVec3(rtrans.scale));}
            else if(collShape->getShapeType()==TRIANGLE_MESH_SHAPE_PROXYTYPE&&mesh&&mesh->type==1)
            {tent->collShape=((Mesh*)mesh)->getCollider(1);tent->collShape->setLocalScaling(toBVec3(rtrans.scale));}
        }
        float mass=rigidBody->getCollisionFlags()&btCollisionObject::CF_STATIC_OBJECT?0:rigidBody->getInvMass();
        btVector3 inertia(0.0,0.0,0.0);
        tent->collShape->calculateLocalInertia(mass,inertia);

        tent->rigidBody=new btRigidBody(*rigidBody);
        tent->rigidBody->setMassProps(mass,inertia);
        tent->rigidBody->setMotionState(tent->motionState);
        tent->rigidBody->setCollisionShape(tent->collShape);
        if(rigidBody->getCollisionFlags()&btCollisionObject::CF_CHARACTER_OBJECT)
        {tent->rigidBody->setActivationState(DISABLE_DEACTIVATION);}
        dynamicsWorld->addRigidBody(tent->rigidBody);
    }
    tent->children.clear();
    Scene::activeScene->entities.push_back(tent);
    if(par&&parent){tent->transform=rtrans;tent->parent=0;parent->addChild(tent);}
    for(int i=0;i<children.size();i++){
        bool tinst=children[i]->source.length();
        tent->addChild(children[i]->clone(false,tinst));
    }
    return tent;
}
void Entity::unparent()
{
    if(parent){
        update(); // to get proper ptrans
        int i=0,ind=-1;
        std::vector<Entity*>& pchildren=parent->children;
        for(i=0;i<pchildren.size();i++){
            if(this==pchildren[i]){ind=i;break;}
        }
        if(ind>-1){
            pchildren.erase(pchildren.begin()+ind);
        }
        transform.multiply(ptrans);
    }
    parent=0;
}
void Entity::addChild(Entity* child)
{
    // remove child from its past parent's children list
    child->unparent();
    
    children.push_back(child);child->parent=this;
    // change transform in terms of new parent
    child->transform=rtrans.inverse()* child->rtrans;
}

Entity * Entity::findEntity(std::string tname,char pre)
{
    Entity* t=0;
    Scene* tscn=Scene::common;
    if(pre){tscn=Scene::prefabs;}
    if(tscn){
        for(int i=0;i<tscn->entities.size();i++){
            if(tname==tscn->entities[i]->name ){t=tscn->entities[i];break;}
        }   
    }
    if(pre){return t;}
    if(!t){
        for(int i=0;i<Scene::activeScene->entities.size();i++){
            if(tname==Scene::activeScene->entities[i]->name ){t=Scene::activeScene->entities[i];break;}
        }
    }
    return t;
}
Entity * Entity::findEntity(btCollisionObject* rb)
{
    Entity* t=0;
    if(Scene::common &&rb){
        for(int i=0;i<Scene::common->entities.size();i++){
            if(rb==Scene::common->entities[i]->rigidBody ){t=Scene::common->entities[i];break;}
        }   
    }
    if(!t && rb){
        for(int i=0;i<Scene::activeScene->entities.size();i++){
            if(rb==Scene::activeScene->entities[i]->rigidBody ){t=Scene::activeScene->entities[i];break;}
        }
    }
    return t;
}
Entity * Entity::findEntity(Entity* ent)
{
    Entity* t=0;
    if(Scene::common &&ent){
        for(int i=0;i<Scene::common->entities.size();i++){
            if(ent==Scene::common->entities[i] ){t=Scene::common->entities[i];break;}
        }   
    }
    if(!t && ent){
        for(int i=0;i<Scene::activeScene->entities.size();i++){
            if(ent==Scene::activeScene->entities[i] ){t=Scene::activeScene->entities[i];break;}
        }
    }
    return t;
}


bool Entity::isEnabled()
{
    bool ten=enabled;
    Entity *par=parent;
    while(par!=0){
        ten=ten&&par->enabled;
        par=par->parent;
    }
    return ten;
}


vec3 toVec3(btVector3 v){return vec3(v.x(),v.y(),v.z());}
quat toQuat(btQuaternion q){return quat(q.x(),q.y(),q.z(),q.w());}
btVector3 toBVec3(vec3 v){return btVector3(v.x,v.y,v.z);}
btQuaternion toBQuat(quat q){return btQuaternion(q.x,q.y,q.z,q.w);}
