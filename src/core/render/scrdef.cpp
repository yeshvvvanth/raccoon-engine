#include"../main/scene.h"
#include"../UI/textfield.h"
#define PI 3.14159265358979323846

/// TODO: check for proper lua_pop calls to remove extra values from stack
void setTableFunc(std::string name,int (*function)(lua_State*)){
  lua_pushstring(Script::L,name.c_str());
  lua_pushcfunction(Script::L,function);
  lua_settable(Script::L,-3);
}

vec3 getvec3(lua_State* L,int index){
     luaL_checktype(L, index, LUA_TTABLE);
     
     lua_getfield(L,index,"type");
     if(lua_tonumber(L,-1)!=110){return vec3(0.0);};
     
     lua_getfield(L,index,"x");
     lua_getfield(L,index,"y");
     lua_getfield(L,index,"z");
     vec3 tv=vec3(lua_tonumber(L,-3),lua_tonumber(L,-2),lua_tonumber(L,-1));
     lua_pop(L,3+1);
    return  tv;
}
quat getquat(lua_State* L,int index){
     luaL_checktype(L, index, LUA_TTABLE);
     
     lua_getfield(L,index,"type");
     if(lua_tonumber(L,-1)!=112){return quat(0.0);};
     
     lua_getfield(L,index,"x");
     lua_getfield(L,index,"y");
     lua_getfield(L,index,"z");
     lua_getfield(L,index,"w");
     quat tq= quat(lua_tonumber(L,-4),lua_tonumber(L,-3),lua_tonumber(L,-2),lua_tonumber(L,-1));
     lua_pop(L,4+1);
    return tq;
}

void setvec3(lua_State* L,vec3& vec){
  
  lua_getglobal(L,"Vec3");
  lua_getfield(L,-1,"new");
  
  lua_pushnumber(L,vec.x);
  lua_pushnumber(L,vec.y);
  lua_pushnumber(L,vec.z);
   
  lua_pcall(L,3,1,0);
  lua_remove(L,-2);
}

void setquat(lua_State* L,quat& quat){
    
  lua_getglobal(L,"Quat");
  lua_getfield(L,-1,"new");
  
  lua_pushnumber(L,quat.x);
  lua_pushnumber(L,quat.y);
  lua_pushnumber(L,quat.z);
  lua_pushnumber(L,quat.w);
   
  lua_pcall(L,4,1,0);
  lua_remove(L,-2);
}
void pushNumber(lua_State* L,std::string name,float value){
        lua_pushstring(L,name.c_str());
        lua_pushnumber(L,value);
        lua_settable(L,-3);
}
void pushBool(lua_State* L,std::string name,bool value){
        lua_pushstring(L,name.c_str());
        lua_pushboolean(L,value);
        lua_settable(L,-3);
}
void pushString(lua_State* L,std::string name,std::string text){
        lua_pushstring(L,name.c_str());
        lua_pushstring(L,text.c_str());
        lua_settable(L,-3);
}
void pushVec3(lua_State* L,std::string name,vec3& vec){
        lua_pushstring(L,name.c_str());
        setvec3(L,vec);
        lua_settable(L,-3);
}

/// Input ////////////////////////////////
int get_Key(lua_State* L){
  int type=lua_tonumber(L,1);
  int k=lua_tonumber(L,2);
  bool b=false;
  if(type==0)b=Input::getKey(k);
  else if(type==1)b=Input::getKeyDown(k);
  lua_pushboolean(L,b);
  return 1;
}
int getScene(lua_State* L){
  
    lua_pushstring(L,Scene::activeScene->name.c_str());
    
  return 1;
}

int pushEntity(lua_State* L,Entity* ent){
//     ent=Entity::findEntity(ent);    
    if(ent){
        lua_getglobal(L,"Entity");
        lua_getfield(L,-1,"new");
        lua_remove(L,-2);
        lua_pushlightuserdata(L,ent);
        lua_pushstring(L,ent->name.c_str());
        lua_pcall(L,2,1,0);
    }
    return 0;
}

/// quat ///////////////////////////////
int slerp(lua_State* L){
    quat tq1=getquat(L,1);
    quat tq2=getquat(L,2);
    float fac=lua_tonumber(L,3);
    quat result=quat::slerp(tq1,tq2,fac);
    setquat(L,result);
    return 1;
}
int fromEuler(lua_State* L){
    if(lua_isnumber(L,1)){
        float x=lua_tonumber(L,1)*PI/180.0f;
        float y=lua_tonumber(L,2)*PI/180.0f;
        float z=lua_tonumber(L,3)*PI/180.0f;
        
        quat tq=quat::FromEulerAngles(vec3(x,y,z));
        setquat(L,tq);
    }
    else{
        vec3 v1=getvec3(L,1);
        vec3 v2=getvec3(L,2);
        quat q=quat(v1,v2);
        /*
        if(v1.Cross(v2).SqMagnitude()==0){
            float dot=v1.Dot(v2);
            if(dot<=0 || dot!=v1.Magnitude()*v2.Magnitude()){
                q=quat();
            }
        }
        else{
            q=quat(v1,v2);
        }
        */
        setquat(L,q);
    }
  return 1;
}
int toEuler(lua_State* L){
  quat tq=getquat(L,1);
  
  vec3 tv=tq.ToEulerAngles();
  tv.x*=180.0f/PI;tv.y*=180.0f/PI;tv.z*=180.0f/PI;
  
  setvec3(L,tv);
  
  return 1;
}
int rotatequatvec3(lua_State* L){
  quat tq=getquat(L,1);
  vec3 v1=getvec3(L,2);
  
  vec3 tv=quat::Rotate(tq,v1);
  
  setvec3(L,tv);
  return 1;
}
mat4 getmat4(lua_State* L,int ind){
    float elem[16];
    lua_getfield(L,ind,"type");
    if(lua_tonumber(L,-1)!=113){return mat4(1.0);}
    for(int i=1;i<16+1;i++){
        lua_pushnumber(L,i);
        lua_gettable(L,ind);
        elem[i-1]=lua_tonumber(L,-1);
    }
    lua_pop(L,16+1);
        
    return mat4(elem);
}
void setmat4(lua_State* L,mat4& mat){
    lua_getglobal(L,"Mat4");
    lua_getfield(L,-1,"new");
    lua_remove(L,-2);
    lua_pushnumber(L,1);
    lua_pcall(L,1,1,0);
    for(int i=1;i<16+1;i++){
        lua_pushnumber(L,i);
        lua_pushnumber(L,mat.elements[i-1]);
        lua_settable(L,-3);
    }
}
int getTrans(lua_State* L){
    int type=lua_tonumber(L,1);
    Entity* ae=Script::aentity;
    ae->update();
    if(type==0){setvec3(L,ae->rtrans.position);}
    else if(type==1){setquat(L,ae->rtrans.rotation);}
    else if(type==2){setvec3(L,ae->rtrans.scale);}
    
    else if(type==13)
    {setmat4(L,Script::aentity->transform.model);}
    
    return 1;
}
int setTrans(lua_State* L){
    int type=lua_tonumber(L,1);
    Entity* ae=Script::aentity;
    bool change=true;
    if(type==0){ae->rtrans.position=getvec3(L,2);}
    else if(type==1){ae->rtrans.rotation=getquat(L,2);}
    else if(type==2){ae->rtrans.scale=getvec3(L,2);}
    else{change=false;}
    if(change){
        ae->rtrans.update();
        ae->transform=ae->ptrans.inverse()*ae->rtrans;
        ae->update();
    }
    if(change&&ae->rigidBody){
        btVector3 pos=ae->rigidBody->getCenterOfMassPosition();
        btQuaternion rot=ae->rigidBody->getCenterOfMassTransform().getRotation();
        if(type==0)
            pos=toBVec3(ae->rtrans.position);
        else if(type==1)
            rot=toBQuat(ae->rtrans.rotation);
        ae->rigidBody->setCenterOfMassTransform(btTransform(rot,pos));
    }
    return 1;
}

int multiplymat4(lua_State* L){
    lua_getfield(L,1,"type");
    lua_getfield(L,2,"type");
    if(lua_tonumber(L,3)!=113)
    {lua_pop(L,2);mat4 tm;setmat4(L,tm);return 1;};
    
    short vtype=lua_tonumber(L,4);
    lua_pop(L,2);
    mat4 tmat=getmat4(L,1);
    if(vtype==110){
        vec3 tvec=tmat*getvec3(L,2);
        setvec3(L,tvec);
    }
    else if(vtype==111){
     
     lua_getfield(L,2,"x");
     lua_getfield(L,2,"y");
     lua_getfield(L,2,"z");
     lua_getfield(L,2,"w");
     vec4 tv=tmat*vec4(lua_tonumber(L,-4),lua_tonumber(L,-3),lua_tonumber(L,-2),lua_tonumber(L,-1));
     //
     lua_pop(L,4);
     lua_getglobal(L,"Vec4");
     lua_getfield(L,-1,"new");
     lua_remove(L,-2);
  
    lua_pushnumber(L,tv.x);
    lua_pushnumber(L,tv.y);
    lua_pushnumber(L,tv.z);
    lua_pushnumber(L,tv.w);
   
    lua_pcall(L,4,1,0);
    }
    else if(vtype==113){
        mat4 tm1=tmat*getmat4(L,2);
        setmat4(L,tm1);
    }
    return 1;
}
/// //////////////////////////////////////////

int setEntity(lua_State* L){
    Entity* tent=(Entity*) lua_touserdata(L,-1);
    if(tent==Script::aentity){return 1;}// no need to check the existing one
    
    Entity* ent=0;
    for(int i=0;i<Scene::activeScene->entities.size();i++){if(Scene::activeScene->entities[i]==tent){ent=tent;break;}}
    if(!ent){
        for(int i=0;i<Scene::common->entities.size();i++){
            if(Scene::common->entities[i]==tent){ent=tent;break;}
        }
    }
    if(ent){
        Script::aentity=ent;
        return 1;
    }
    return 0;
}
int getEntity(lua_State* L){
    Entity* tent;
    std::string ename=lua_tostring(L,-1);
    tent=Entity::findEntity(ename);
    if(tent){
        pushEntity(L,tent);
    }
    else {lua_pushnil(L);}
    return 1;
}


/// /////////////////////

int getSound(lua_State* L){
    Sound* sound=Script::aentity->sound;
    if(sound){
        lua_newtable(L);
        
        pushNumber(L,"gain",sound->gain);
        pushNumber(L,"pitch",sound->pitch);
        pushBool(L,"loop",sound->flags&LOOP);
        pushBool(L,"playing",sound->playing);
    }
    else{lua_pushnil(L);}
    return 1;
}

int setSound(lua_State* L){
    Sound* sound=Script::aentity->sound;
    if(sound){                              // 1,2,3,4 == -4,-3,-2,-1 // 1 never changes
        if(lua_isboolean(L,1))
        {
            bool play=lua_toboolean(L,1),stop=lua_toboolean(L,2);
            if(play)sound->Play();
            else sound->Pause(stop);
        }
        else if(lua_istable(L,1)){
            lua_getfield(L,1,"gain");
            lua_getfield(L,1,"pitch");
            lua_getfield(L,1,"loop");
            sound->gain=lua_tonumber(L,2);
            sound->pitch=lua_tonumber(L,3);
            if(lua_toboolean(L,4))sound->flags|=LOOP;else sound->flags&=~LOOP;
            lua_pop(L,3);
//             printf("loop = %d \n",sound->loop);
//             printf("pitch = %f \n",sound->pitch);
        }
    }
    return 0;
}
int getLight(lua_State* L){
    Light* light=Script::aentity->light;
    if(light){
        lua_newtable(L);
        
        pushVec3(L,"colour",light->colour); 
        pushNumber(L,"type",light->type);
        pushNumber(L,"intensity",light->intensity);
        pushNumber(L,"radius",(light)->radius);
        pushNumber(L,"angle",(light)->angle);
    }
    else{lua_pushnil(L);}
    return 1;
}

int setLight(lua_State* L){
    Light* light=Script::aentity->light;
    if(lua_isboolean(L,1)){                              // 1,2,3,4 == -4,-3,-2,-1 // 1 never changes
        vec3 col=getvec3(L,2);
        Light::setAmbient(Shader::pervert,col);
        Light::setAmbient(Shader::def,col);
    }
    else{
        if(light){
            lua_getfield(L,1,"colour");
            lua_getfield(L,1,"type");
            lua_getfield(L,1,"intensity");
            lua_getfield(L,1,"radius");
            lua_getfield(L,1,"angle");
            
            light->colour=getvec3(L,2);
            light->type=lua_tonumber(L,3);
            light->intensity=lua_tonumber(L,4);
            light->radius=lua_tonumber(L,5);
            light->angle=lua_tonumber(L,6);
            lua_pop(L,5);
        }
    }
    return 0;
}
int getRigidBody(lua_State* L){
    Entity* ent=Script::aentity;
    btRigidBody* rb=ent->rigidBody;
    short astate=0;
    if(rb){
        if(rb->getActivationState()&ISLAND_SLEEPING){astate=1;}
        else if(rb->getActivationState()&WANTS_DEACTIVATION){astate=2;}
        btTransform btrans;
        rb->getMotionState()->getWorldTransform(btrans);
        vec3 pos=toVec3(btrans.getOrigin()),
        avel=toVec3(rb->getAngularVelocity()),
        lvel=toVec3(rb->getLinearVelocity());
        quat rot=toQuat(btrans.getRotation());
        vec3 gravity=toVec3(rb->getGravity());
        
        lua_getglobal(L,"RigidBody");
        lua_getfield(L,-1,"new");
        lua_remove(L,-2);

        lua_pushlightuserdata(L,rb);
        lua_pushnumber(L,rb->getCollisionShape()->getShapeType());
        lua_pushnumber(L,1/rb->getInvMass());    
        lua_pushnumber(L,rb->getFriction());      
        setvec3(L,pos);
        setquat(L,rot);
        lua_pushnumber(L,rb->getLinearDamping());         
        lua_pushnumber(L,rb->getAngularDamping());         
        setvec3(L,lvel);
        setvec3(L,avel);
        setvec3(L,gravity);
        lua_pushnumber(L,astate);
        
        lua_pcall(L,12,1,0); 
    }
    else{lua_pushnil(L);}
    return 1;
}
int setRigidBody(lua_State* L){
    Entity* ent=Script::aentity;
    btRigidBody* rb=ent->rigidBody;
    if(rb&&ent->parent&&ent->parent->rigidBody&&
        ent->rigidBody->getCollisionShape()==ent->parent->rigidBody->getCollisionShape())
    {
        rb=ent->parent->rigidBody;
    }
    if(rb){
        vec3 pos,avel,lvel,gravity;
        quat rot;
        float mass,ld,ad,friction;
        
        lua_getfield(L,1,"udata");
        lua_getfield(L,1,"mass");
        lua_getfield(L,1,"friction");
        lua_getfield(L,1,"linearDamping");
        lua_getfield(L,1,"angularDamping");
        lua_getfield(L,1,"linearVelocity");
        lua_getfield(L,1,"angularVelocity");
        lua_getfield(L,1,"gravity");
//         lua_getfield(L,1,"pos");
//         lua_getfield(L,1,"rot");

//         rb=(btRigidBody*)lua_touserdata(L,2);
        mass=lua_tonumber(L,3);
        friction=lua_tonumber(L,4);
        ld=lua_tonumber(L,5);
        ad=lua_tonumber(L,6);
        lvel=getvec3(L,7);
        avel=getvec3(L,8);
        gravity=getvec3(L,9);
        lua_pop(L,8);
//         pos=getvec3(L,4);
//         rot=getquat(L,5);
        if(mass!=1/rb->getInvMass()){
            btVector3 fallInertia(0.0,0.0,0.0);
            ent->collShape->calculateLocalInertia(mass, fallInertia);
            rb->setMassProps(mass,fallInertia);
        }
        if(friction!=rb->getFriction())rb->setFriction(friction);
        if(ld!=rb->getLinearDamping()||ad!=rb->getAngularDamping())
            rb->setDamping(ld,ad);
        if(avel!=toVec3(rb->getAngularVelocity()))rb->setAngularVelocity(toBVec3(avel));
        if(lvel!=toVec3(rb->getLinearVelocity()))rb->setLinearVelocity(toBVec3(lvel));
        if(toBVec3(gravity)!=rb->getGravity()){rb->setGravity(toBVec3(gravity));}
    }
    else{lua_pushnil(L);}
    return 1;
}
int applyRBody(lua_State* L){
    Entity* ent=Script::aentity;
    btRigidBody* rb=ent->rigidBody;
    if(rb&&ent->parent&&ent->parent->rigidBody&&
        ent->rigidBody->getCollisionShape()==ent->parent->rigidBody->getCollisionShape())
    {
        rb=ent->parent->rigidBody;
    }
    
    int type=lua_tonumber(L,1);
    vec3 tv;btVector3 bv,rp(0,0,0);float f=0;
    
    if(type==6){f=lua_tonumber(L,2);}
    else{
        tv=getvec3(L,2);
        bv=btVector3(tv.x,tv.y,tv.z);
        
    }
    if(rb){
        switch (type){
            case 0:
                rb->applyCentralImpulse(bv);
                break;
            case 1:
                tv=getvec3(L,3);
                rp=btVector3(tv.x,tv.y,tv.z);
                rb->applyImpulse(bv,rp);
                break;
            case 2:
                rb->applyCentralForce(bv);
                break;
            case 3:
                tv=getvec3(L,3);//-ent->transform.position;
                rp=btVector3(tv.x,tv.y,tv.z);
                rb->applyForce(bv,rp);
                break;
            case 4:
                rb->applyTorqueImpulse(bv);
                break;
            case 5:
                rb->applyTorque(bv);
                break;
            case 6:
                rb->applyDamping(f);
                break;
        }
    }
    return 0;
}

int getMouse(lua_State* L){
    int type=lua_tonumber(L,1);
    vec3 tv;bool b=true;

    if(type==0)tv=vec3(Input::getMousedelta().x,Input::getMousedelta().y,0.0);
    else if(type==1){tv=Camera::active_camera->getCamRay();}
    else if(type==2)tv=vec3(Shader::scrMousePos(true).x,Shader::scrMousePos(true).y,0.0);
    else if(type==3)tv=vec3(Shader::getResolution().x,Shader::getResolution().y,0.0);
    else if(type==4){vec3 tp=getvec3(L,2);tv=Camera::active_camera->getCamRay(true,tp.x,tp.y);}
    else if(type==7){tv=Camera::active_camera->getCamPos();}
    else if(type==8)tv=vec3(Input::getMouseX(),Input::getMouseY(),0.0);
    else{b=false;} if(b)setvec3(L,tv);
    
    if(type==5){lua_pushboolean(L,Input::getMouseButton(lua_tonumber(L,2)));}
    else if(type==6){lua_pushboolean(L,Input::getMouseButtonDown(lua_tonumber(L,2)));}
    else if(type==9){lua_pushnumber(L,Input::scroll);}
    
    return 1;
}

int interSphereBox(lua_State*L){
    vec3 tv=getvec3(L,1);
    float r=lua_tonumber(L,2);
    bool ins=false;float dist;
    if(Script::aentity->mesh){
        Script::aentity->mesh->box.tick(Script::aentity->transform);
        ins=Script::aentity->mesh->box.IntersectSphere(tv,r*r,dist);
    }
    lua_pushboolean(L,ins);
    return 1;
}

int parchild(lua_State* L){
    bool bp=lua_toboolean(L,1);
    Entity* tent=Script::aentity->parent;
    int ret=0;
    if(bp && tent){
        pushEntity(L,tent);
        ret++;
    }
    else if(!bp){
        short chnum=Script::aentity->children.size();
        if(chnum>0){
            ret++;
            lua_newtable(L);
            
            lua_pushstring(L,"size");
            lua_pushnumber(L,chnum);
            lua_settable(L,-3);
            
            for(int i=1;i<chnum+1;i++){
                tent=Script::aentity->children[i-1];
                lua_pushnumber(L,i);
                pushEntity(L,tent);
                
                lua_settable(L,-3);
            }
            
        }
    }

    return ret;
}
int getEnt(lua_State* L){
    Entity* tent=Script::aentity;
    
    return 1;
}
int getEntities(lua_State*L){
    lua_newtable(L);
    int ind=lua_gettop(L);
    int esize=Scene::activeScene->entities.size();
    lua_pushstring(L,"size");
    lua_pushnumber(L,esize);
    lua_settable(L,-3);
    setTableFunc("find",getEntity);
    for(int i=1;i<esize+1;i++){
        Entity* tent=Scene::activeScene->entities[i-1];
        lua_pushnumber(L,i);
        pushEntity(L,tent);
        
        lua_settable(L,ind);
    }
    return 1;
}
int setActiveScene(lua_State* L){
    int type=5,num=0;
    std::string name="";
    type=lua_tonumber(L,1);
    num=lua_tonumber(L,2);
    bool reload=lua_toboolean(L,3);
    name=lua_tostring(L,4);
    bool b=Scene::setScene(type,num,reload,name);
    lua_pushboolean(L,b);
    return 1;
}
int getButton(lua_State* L){
    Button* but=Script::aentity->button;
    if(but){
        lua_newtable(L);
            pushNumber(L,"type",but->type);
            pushBool(L,"justDown",but->justDown);
            pushBool(L,"toggle",but->toggle);
            pushNumber(L,"width",but->transform.scale.x);
            pushNumber(L,"height",but->transform.scale.y);
            pushNumber(L,"x",but->transform.position.x);
            pushNumber(L,"y",but->transform.position.y);
            if(but->type==2)     pushNumber(L,"value",((Slider*)but)->value);
            else if(but->type==4)pushString(L,"text",((TextField*)but)->label->text.c_str());
            else if(but->type==5)pushString(L,"text",((TextButton*)but)->label->text.c_str());
    }
    else{
        lua_pushnil(L);
    }
    return 1;
}
int setButton(lua_State* L){
    Button* but=(Button*)Script::aentity->button;
    char type=lua_tonumber(L,1);
    if(but){
        if(type==1){but->transform.scale.x=lua_tonumber(L,2);}
        else if(type==2){but->transform.scale.y=lua_tonumber(L,2);}
        else if(type==3){but->transform.position.x=lua_tonumber(L,2);}
        else if(type==4){but->transform.position.y=lua_tonumber(L,2);}
        else if(type==5 && lua_isstring(L,2)){
            if(but->type==1)((TextButton*)but)->label->setText(lua_tostring(L,2));// textbutton
            if(but->type==4)((TextField*) but)->label->setText(lua_tostring(L,2));// field
            if(but->type==5)((TextButton*)but)->label->setText(lua_tostring(L,2));// lablel
        }
        else if(type==6&&lua_istable(L,2)){ // setting tha colour
            but->uColor=getvec3(L,2);
        }
        else if(type==7&&lua_isnumber(L,2)){ // setting tha colour
            but->alpha=lua_tonumber(L,2);
        }
        but->setPos(but->transform.position.x,but->transform.position.y);
    }
    return 0;
}
int getProp(lua_State* L){
    int ret=0;ScriptProp* sp=0;
    std::string tname=lua_tostring(L,1);
    bool pub= tname[0]=='_';
    if(pub)tname=tname.substr(1,tname.length()-1);
    if(Script::aentity->script){
        std::vector<ScriptProp*> *props=&Script::aentity->script->p_props;
        if(pub)props=&Script::aentity->script->props;
        for(int l=0;l<props->size();l++){
            sp=props->at(l);
            if(sp->name==tname){ret++;break;}
        }
    }
    if(ret){
        if(sp->type==1){ lua_pushnumber(L,((SFloat*)sp)->value);}
        else  if(sp->type==2){ lua_pushboolean(L,((Sbool*)sp)->value);}
        else  if(sp->type==3){ lua_pushstring(L,((Sstring*)sp)->value.c_str());}
        else ret=0;
    }
    return ret;
}
int setProp(lua_State* L){
    std::string key=lua_tostring(L,1);
    ScriptProp* sp=0;bool found=false;
    if(Script::aentity->script){
        for(int l=0;l<Script::aentity->script->p_props.size();l++){
            sp=Script::aentity->script->p_props[l];
            if(sp->name==key){found=true;break;}
        }
    }
    if(found){
        if(lua_isnumber(L,2))
            ((SFloat*)sp)->value=lua_tonumber(L,2);
        else if(lua_isboolean(L,2))
            ((Sbool*)sp)->value=lua_toboolean(L,2);
        else if(lua_isstring(L,2))
            ((Sstring*)sp)->value=lua_tostring(L,2);
        sp->update(&Script::aentity->script->name);
    }
    return 0;
}
int setAnim(lua_State* L){
    Entity* ae=Script::aentity;
    if(ae&&ae->mesh&&ae->mesh->type==1){
        Mesh* me=(Mesh*)Script::aentity->mesh;
        if(me->rawmesh->type&MESH_ANIM){
            int type=lua_tonumber(L,1);
            if(type==0&& lua_isnumber(L,2)){// animation by number
                me->setAnimation(lua_tonumber(L,2));
            }
            else if(type==0&&lua_isstring(L,2)){ // animation by name
                me->setAnimation(-1,lua_tostring(L,2));
            }
            else if(type==1){// animation speed
                me->animSpeed=lua_tonumber(L,2);
            }
            else if(type==2){// transition speed
                me->transSpeed=lua_tonumber(L,2);
            }
                lua_pushboolean(L,true);
        }
        else{
            lua_pushboolean(L,false);
        }
        return 1;
    }
    else{ lua_pushboolean(L,false);;return 1;}
}
int do_entity(lua_State* L){
    Entity* ae=(Entity*)lua_touserdata(L,1),*te;
    int type=lua_tonumber(L,2);
    
    if(type==1){        // instantiate 
        te=ae->clone();te->enabled=true;
        te->unparent();
        if(lua_istable(L,3)){
            printf(" instantiated !! \n");
            lua_getfield(L,3,"type");
            type=lua_tonumber(L,-1);
            lua_pop(L,1);
            if(type==110){
                te->rtrans.position=te->transform.position=getvec3(L,3);
                if(te->rigidBody){
                    btTransform et(toBQuat(te->transform.rotation),toBVec3(te->transform.position));
                    te->rigidBody->setCenterOfMassTransform(et);
                }
                te->update();// sets rtrans from transform
            }
            else if(type==114){
                lua_getfield(L,3,"udata");
                Entity* tent= (Entity*)lua_touserdata(L,-1);
                te->transform=tent->rtrans;
                lua_pop(L,1);
            }
        }
        pushEntity(L,te);
        return 1;
    }
    else if(type==0){
        // add to deleting list if not already added
        bool dex=false;
        for(int h=0;h<Scene::activeScene->dentities.size();h++){
            if(ae==Scene::activeScene->dentities[h]){dex=true;break;}
        }
        if(!dex)Scene::activeScene->dentities.push_back(ae);
    }
    else if(type==2&&lua_isstring(L,3)){
        //rename entity
        ae->name=lua_tostring(L,3);
    }
    else if(type==3&&lua_isboolean(L,3)){
        ae->enabled=lua_toboolean(L,3);
    }
    return 0;
}
int pickEntity(lua_State* L){
    Entity *sel=Scene::PickEntity();
    if(sel){
        pushEntity(L,sel);
    }
    else{lua_pushnil(L);}
    return 1;
}
int CastRay(lua_State* L){
    vec3 v1=getvec3(L,1);
    vec3 v2=getvec3(L,2);
    short retnum=0,type=lua_tonumber(L,3);
    bool raycast=type&1,drawline=type&2,closest=type&4,boolean=type&8;
    // do ray casting   //   (1= raycast) (2= draw line) (4= closest) (8= boolean)
    if(raycast){ // cast ray
        Entity* cent=0;vec3 point,normal;
        if(closest){// closest object
            btCollisionWorld::ClosestRayResultCallback	closestResults(toBVec3(v1),toBVec3(v2));
            closestResults.m_flags|=1<<0;
            Entity::dynamicsWorld->rayTest(toBVec3(v1),toBVec3(v2),closestResults);
            bool hit=closestResults.hasHit();
            if(hit){
                v2=point=toVec3(closestResults.m_hitPointWorld);
                normal=toVec3(closestResults.m_hitNormalWorld);
            }
            if(boolean){
                lua_pushboolean(L,hit);
                retnum=1;
            }
            else if (hit)
            {
                const btCollisionObject* rb=closestResults.m_collisionObject;
                for(int i=0;i<Scene::activeScene->entities.size();i++){
                    if(Scene::activeScene->entities[i]->rigidBody==rb){
                        cent=Scene::activeScene->entities[i];break;
                    }
                }
                for(int i=0;i<Scene::common->entities.size();i++){
                    if(Scene::common->entities[i]->rigidBody==rb){
                        cent=Scene::common->entities[i];break;
                    }
                }
                if(cent){
                    // return entity
                    pushEntity(L,cent);
                    setvec3(L,point);
                    setvec3(L,normal);
                    retnum=3;
                }
            }
            else{
                retnum=0;
            }
        }
        else{ // all objects 
            btCollisionWorld::AllHitsRayResultCallback allResults(toBVec3(v1),toBVec3(v2));
            Entity::dynamicsWorld->rayTest(toBVec3(v1),toBVec3(v2),allResults);
            for (int i=0;i<allResults.m_hitFractions.size();i++)
            {
                // do ya thang!
            }
            return 0;
        }
    }
    // draw line 
    if(drawline){
        vec3 colour=vec3(1.0,0.0,0.0);
        if(lua_istable(L,4))colour=getvec3(L,4);
        Shader::drawLine(v1,v2,colour);
    }

    return retnum;
}
int C_drawPrim(lua_State* L){
    int type=lua_tonumber(L,1);
    if(type==-1){
        vec3 sp=getvec3(L,2);
        vec3 ep=getvec3(L,3);
        float th1=lua_tonumber(L,4);
        float th2=lua_tonumber(L,5);
        vec3 sc=getvec3(L,6);
        vec3 ec=getvec3(L,7);
        float a1=lua_tonumber(L,8);
        float a2=lua_tonumber(L,9);
        Shader::renderLine(sp,ep,th1,th2,vec4(sc,a1),vec4(ec,a2));
    }
    else{
        Transform tr;
        vec3 sp=getvec3(L,2);
        vec3 scale=getvec3(L,3);
        vec3 sc=getvec3(L,4);
        float a1=lua_tonumber(L,5);
        tr.position=sp;tr.scale=scale;tr.update();
        Shader::renderPrim(type,tr,vec4(sc,a1));
    }
    return 0;
}
int setglow(lua_State* L){
    vec3 tglow=getvec3(L,1);
    if(Script::aentity->mesh&&Script::aentity->mesh->type==1)
        ((Mesh*)Script::aentity->mesh)->glow=tglow;
    return 0;
}
int removeComp(lua_State* L){
    short type=lua_tonumber(L,1);
    Script::aentity->removeComp(type);
    return 0;
}
int setGravity(lua_State* L){
    vec3 grav=getvec3(L,1);
    Entity::dynamicsWorld->setGravity(toBVec3(grav));
    return 0;
}

int sphereTest(lua_State* L){
    vec3 spos=getvec3(L,1);
    float radius=lua_tonumber(L,2);
    btRigidBody* avoid=Script::aentity->rigidBody;
//     if(lua_islightuserdata(L,3)){avoid=(btRigidBody*)lua_touserdata(L,3);}
    int renum=0;
    
    //[1]
    btSphereShape sphere(radius);
    btCollisionObject ghost;
    btTransform xform(btQuaternion(0,0,0,1),toBVec3(spos));
    
    ghost.setCollisionShape(&sphere);
    ghost.setWorldTransform(xform);
    ContactTestCallback callback(&ghost,avoid);
    Entity::dynamicsWorld->contactTest(&ghost,callback);
    /*
    if(callback.colls.size()){
        lua_pushboolean(L,true);renum++;
    }
    */
    for(int i=0;i<callback.colls.size();i++){
//         btCollisionObject* coll=callback.colls[i];
        Entity* tent=Entity::findEntity(callback.colls[i]);
        if(tent){
            pushEntity(L,tent);
            renum++;
        }
    }
    /*
    */
    return renum;
}
int getPath(lua_State* L){
    vec3 v1=getvec3(L,1),v2=getvec3(L,2);
    std::vector<vec3> points;
    bool found=Scene::activeScene->getPath(v1,v2,points);

    short numpoints=points.size();
    lua_newtable(L);
        pushNumber(L,"curr",0);
    if (found){
        pushBool(L,"finished",false);
        for(int i=0;i<numpoints;i++){
            printf("@@@@nagasake\n");
            lua_pushnumber(L,i);
            setvec3(L,points[i]);
            lua_settable(L,-3);
        }
    }else{
        pushBool(L,"finished",true);
        lua_pushnumber(L,0);
        setvec3(L,v1);
        lua_settable(L,-3);
    }

    return 1;
}
int gprint(lua_State* L){
    vec3 pos;float scale=18;
    std::string str=lua_tostring(L,1);
    if(lua_isnumber(L,2))pos.x=lua_tonumber(L,2);
    if(lua_isnumber(L,3))pos.y=lua_tonumber(L,3);
    Label& label=*(Label::label);
    label.x=pos.x;label.y=pos.y;
    label.scale=scale;
    label.setText(str);
    label.tick();
    printf("gprinting!!\n");
    return 0;
}
/// //////////////////////////////////////////
void registerClasses(){
  lua_State* L=Script::L;
  lua_register(L,"setglow",setglow);
  
  lua_register(L,"getScene",getScene);
  lua_register(L,"do_entity",do_entity);
  lua_register(L,"get_Key",get_Key);

  lua_register(L,"getTrans",getTrans);
  lua_register(L,"setTrans",setTrans);
  lua_register(L,"setEntity",setEntity);
  lua_register(L,"findEntity",getEntity);
  lua_register(L,"getEntities",getEntities);
  lua_register(L,"parchild",parchild);
  
  lua_register(L,"setActiveScene",setActiveScene);
  lua_register(L,"getLight",getLight);
  lua_register(L,"setLight",setLight);
  lua_register(L,"getSound",getSound);
  lua_register(L,"setSound",setSound);
  lua_register(L,"setAnimation",setAnim);
  
  lua_register(L,"getButton",getButton);
  lua_register(L,"setButton",setButton);
  lua_register(L,"getProp",getProp);
  lua_register(L,"setProp",setProp);
  lua_register(L,"getRigidBody",getRigidBody);
  lua_register(L,"setRigidBody",setRigidBody);
  lua_register(L,"applyRBody",applyRBody);
  lua_register(L,"C_removeComp",removeComp);
  
  lua_register(L,"multiplymat4",multiplymat4);
   
  lua_register(L,"getMouse",getMouse);
  lua_register(L,"interSphereBox",interSphereBox);
  
  lua_register(L,"pickEntity",pickEntity);
  lua_register(L,"C_castRay",CastRay);
  lua_register(L,"C_drawPrim",C_drawPrim);
  lua_register(L,"setGravity",setGravity);
  lua_register(L,"sphereTest",sphereTest);
  lua_register(L,"getPath",getPath);
  lua_register(L,"printf",gprint);
  
  lua_pop(L,1);
  lua_getglobal(L,"Quat");
    setTableFunc("fromEuler",fromEuler);
    setTableFunc("toEuler",toEuler);
    setTableFunc("rotate",rotatequatvec3);
    setTableFunc("slerp",slerp);
  lua_pop(L,1);
}
