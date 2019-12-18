#include"scene.h"
#include"../utils/data.h"

Scene Scene::def_scene;
Scene *Scene::activeScene=&def_scene,*Scene::next=0,*Scene::common=0,*Scene::prefabs=0;
std::vector<Scene*> Scene::scenes;
std::vector<Scene*> Scene::ascenes;
bool Scene::play=false,Scene::reload=false;
float navtime=0.01;
Scene::Scene(std::string tname){
    name=tname;
    swap=0;camera=0;
    ///  replace this stupid tactic with something else!
//     for(int i=0;i<scenes.size();i++){if(name==scenes[i]->name)name=name+"1";}
    if(this==&def_scene)name="default_scene";
}
Scene::Scene(Data* dat){
    this->data=dat;
    name="new_scene";
    swap=0;camera=0;
    for(int i=0;i<scenes.size();i++){if(name==scenes[i]->name)name=name+"1";}
    if(this==&def_scene)name="default_scene";
}
Scene::~Scene()
{
  if(swap)delete swap;
  if(activeScene==this)activeScene=0;
  for(unsigned short u=0;u<entities.size();u++)
  {delete entities[u];}
  entities.clear();
}
std::vector<Entity*> pcolents1,pcolents2;
void Scene::tick()
{
    std::vector<Entity*> colents1,colents2;std::vector<bool> newcoll;
    lua_State* L=Script::L;unsigned short u=0;
    // Time
    lua_getglobal(L,"Time");
        lua_pushstring(L,"deltaTime");
        lua_pushnumber(L,Input::deltaTime);
    lua_settable(L,-3);
        lua_pushstring(L,"time");
        lua_pushnumber(L,Input::time);
    lua_settable(L,-3);
        lua_pushstring(L,"timescale");
        lua_pushnumber(L,Input::timescale);
    lua_settable(L,-3);
        lua_pushstring(L,"dt");
        lua_pushnumber(L,Input::timescale*Input::deltaTime);
    lua_settable(L,-3);
    lua_pop(L,1);
if(Input::timescale){

    // checking collisions
    int numManifolds = Entity::dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i=0;i<numManifolds;i++)
    {
        btPersistentManifold* contactManifold =  Entity::dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btRigidBody* rbA = (btRigidBody*) contactManifold->getBody0();
        btRigidBody* rbB = (btRigidBody*) contactManifold->getBody1();
        Entity *e1=0,*e2=0;
        int numContacts = contactManifold->getNumContacts();
        if(numContacts){
            e1=Entity::findEntity(rbA);
            e2=Entity::findEntity(rbB);
        }
        if(e1&&e2){
            colents1.push_back(e1);colents2.push_back(e2);
            colents1.push_back(e2);colents2.push_back(e1);
            newcoll.push_back(true);
            newcoll.push_back(true);
        }
        /*
        for (int j=0;j<numContacts;j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance()<0.f)
            {
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                const btVector3& normalOnB = pt.m_normalWorldOnB;
            }
        }
        */
    }
    for(u=0;u<colents1.size();u++){
        for(int v=0;v<pcolents1.size();v++){
            if(colents1[u]==pcolents1[v]&&colents2[u]==pcolents2[v])
            {newcoll[u]=false;break;}
        }
    }
   for(u=0;u<colents1.size();u++){
      Entity* ent=colents1[u],*cent=colents2[u];
      if(ent->rigidBody&&newcoll[u]){
        if(ent->script){
            Script::setAEntity(ent);// lua: entity = ent
            ent->script->Update(1);ent->script->Update(3);  // update props
            
            lua_getglobal(L,ent->script->name.c_str());
            lua_pushstring(L,"OnCollisionEnter");
            lua_gettable(L,-2);

            if(lua_isfunction(L,-1)){
                lua_newtable(L);
                        lua_pushstring(L,"this");
                        lua_getglobal(L,"entity");// lua: this=entity
                    lua_settable(L,-3);
                        lua_pushstring(L,"other");
                        lua_getglobal(L,"Entity");
                            lua_getfield(L,-1,"new");
                            lua_remove(L,-2);
                            lua_pushlightuserdata(L,cent);
                            lua_pushstring(L,cent->name.c_str());
                            lua_pcall(L,2,1,0);
                    lua_settable(L,-3);
                int ret=lua_pcall(L,1,0,0);
                if(ret){
                    printf("Error: %s \n\n\n", lua_tostring(L,-1));
                    lua_pop(L,1);
                }
            }
            else{lua_pop(L,1);}
            lua_pop(L,1);
            ent->script->Update(0);ent->script->Update(2);    // read props
        }
      }
   }
   pcolents1=colents1;pcolents2=colents2;

   // getting transforms from physics
   for(u=0;u<entities.size();u++){
        entities[u]->tick();
   }
   // frustum culling and shadow buffer rendering
   refresh();
   render();
}
   //runnning scripts
   for(u=0;u<entities.size();u++){
       if(entities[u]->isEnabled()){
          if(entities[u]->script){
              entities[u]->script->entity=entities[u];
              entities[u]->script->tick();
          }
       }
   }
   // ticking Scripts,ui,sounds,lights etc
   for(u=0;u<entities.size();u++){
    entities[u]->update();
   }
   for(u=0;u<entities.size();u++){
        if(entities[u]->camera==Camera::active_camera)
            {camera->update(&entities[u]->rtrans);}
   }

   //rendering them UI thingies
   for(u=0;u<entities.size();u++){
       Entity* go=entities[u];
        if(go->isEnabled()&&go->button){
            if(go->button->type==2)((Slider*)go->button)->tick();
            else if(go->button->type==5||go->button->type==1){((TextButton*)go->button)->tick(true);}
            else go->button->tick(true);
//             if(go->button->type==6)
            go->button->transform.rotation=go->transform.rotation;
        }
   }
   if(common&&this!=common&& this!=common->swap)common->tick();
   //deleting entities that need to be deleted
   for(u=0;u<dentities.size();u++){
       Entity::remove(dentities[u]);
   }
   dentities.clear();
   if(next){
       printf("next scene= %s \n",next->name.c_str());
       if(next->swap&&!reload)
           next->swap->setActive();
       else
           next->Play();
       next=0;
   }
   //
   lua_getglobal(L,"Time");
       lua_getfield(L,-1,"timescale");
       Input::timescale=lua_tonumber(L,-1);
       lua_pop(L,1);
   lua_pop(L,1);
}

Scene * Scene::findScene(std::string tname)
{
    Scene *s=0;
    for(unsigned short u=0;u<scenes.size();u++){
      if(scenes[u]->name==tname){s=scenes[u];break;}
    }
    return s;
}


void Scene::read(std::ifstream& ifs)
{
     unsigned char n_len=0;char* t_name;unsigned short num_go=0;

     ifs.read((char*)&n_len,1);
     t_name=new char[n_len+1];t_name[n_len]='\0';
     ifs.read((char*)t_name,n_len);
     this->name.assign(t_name,strlen(t_name));
     this->name.erase(name.end()-6,name.end());//removing ".scene " extension
     delete[] t_name;

//      printf("^^^*******name of scene = %s *********\n",this->name.c_str());

     ifs.read((char*)&num_go,2);
     for(unsigned short u=0;u<num_go;u++){
       this->entities.push_back(new Entity("Entity"));
     }
     for(unsigned short u=0;u<num_go;u++){
	    entities[u]->read(ifs,this);
        if(!camera&&entities[u]->camera){camera=entities[u]->camera;}
     }
     for(unsigned short u=0;u<num_go;u++){
        entities[u]->update();
     }
}

void cleanGridmaps(Scene* scn){
    Scene* ta=Scene::activeScene;
    
    int esiz=Scene::activeScene->entities.size();
    std::vector<Entity*> gmaps;
    for(int i=0;i<esiz;i++){
        Entity* tnt=Scene::activeScene->entities[i];
        if(tnt->gridmap)gmaps.push_back(tnt);
    }
    esiz=gmaps.size();
    for(int i=0;i<esiz;i++){
        GridMap::clean(gmaps[i]);
    }
    Scene::activeScene=ta;
}


void Scene::write(std::string s_dir)
{
     std::string te_name= (name=="")?"boo.scene":name+".scene";
     std::ofstream ofs(s_dir+te_name,std::ios::binary);
  
     cleanGridmaps(this);
     unsigned short num_go=entities.size();
     ofs.write((char*)&num_go,2);
     for(unsigned short u=0;u<num_go;u++){
            entities[u]->write(ofs,this);
     }
     ofs.close();
}
//rigidbidies get transform from entity on startup    
void setRbtrans(Scene* scn){
    for(int i=0;i<scn->entities.size();i++){
        Entity* nt=scn->entities[i];           
        if(nt->rigidBody){
            nt->update();
            btTransform toff=nt->motionState->m_centerOfMassOffset.inverse();
            nt->motionState->
            setWorldTransform(btTransform(toBQuat(nt->rtrans.rotation),toBVec3(nt->rtrans.position))*toff);
            nt->rigidBody->setMotionState(nt->motionState);
            // add only if NOT in world cuz it is probably added in instPrefabs
            if(!nt->rigidBody->isInWorld())      
            {Entity::dynamicsWorld->addRigidBody(nt->rigidBody);}
        }
        if(nt->camera)scn->camera=nt->camera;
    }

}
void instGridmaps(){
        int esiz=Scene::activeScene->entities.size();
        std::vector<Entity*> gmaps;
        for(int i=0;i<esiz;i++){
            Entity* tnt=Scene::activeScene->entities[i];
            if(tnt->gridmap)gmaps.push_back(tnt);
        }
        esiz=gmaps.size();
        for(int i=0;i<esiz;i++){
            Entity* tnt=gmaps[i];
            if(tnt->gridmap)tnt->gridmap->refresh(tnt);
        }
    
}

void instPrefabs(Scene* scn=0){
        Scene* tmp=Scene::activeScene;
        if(scn)Scene::activeScene=scn;
        std::vector<Entity*> ties;
        
        instGridmaps();
        
        int esiz=Scene::activeScene->entities.size();
        for(int i=0;i<esiz;i++){
            Entity* tnt=Scene::activeScene->entities[i];
            
            // instantiating prefabs   that do not belong to gridmap
            if(tnt->source.length()  &&(!tnt->parent||!tnt->parent->gridmap)){
                Entity* tct= Entity::findEntity(tnt->source,1);
                if(tct){
                    Entity* nt=tct->clone(true,true);
                    if(tnt->parent){tnt->parent->addChild(nt);}
                    nt->source=tnt->source;
                    nt->transform=tnt->transform;nt->transform.scale=tct->transform.scale;
                    nt->transform.update();nt->update();
                    nt->name=tnt->name;
                }
                ties.push_back(tnt);
            }
        }
        int tesiz=ties.size();
        for(int i=0;i<tesiz;i++){
                Entity::remove(ties[i]);
        }
        if(scn)Scene::activeScene=tmp;
}
bool firstLoadScene=true;
void Scene::setActive()
{
    Scene* sc=activeScene;
    activeScene=this;
    if(sc && sc!=this){
        int numrbs=Entity::dynamicsWorld->getNumCollisionObjects();
        btCollisionObjectArray bodies= Entity::dynamicsWorld->getCollisionObjectArray();
        /*
        for(int i=0;i<numrbs;i++){
            {Entity::dynamicsWorld->removeCollisionObject(bodies[i]);}
        }
        */
        for(int i=0;i<sc->entities.size();i++){
         if(sc->entities[i]->rigidBody)
             Entity::dynamicsWorld->removeRigidBody(sc->entities[i]->rigidBody);
        }
    }
    ////////ss/////////////
    instPrefabs();
    setRbtrans(this);
    if(this!=common&&this!=prefabs&&(!Scene::play||firstLoadScene)){
        instPrefabs(common);
    }
    setRbtrans(common);
    /////////ee//////
    if(camera){Camera::active_camera=camera;}
    else if(common&&common->camera){Camera::active_camera=common->camera;}
    else{Camera::active_camera=Camera::default_camera;}

    //bake Navigation
    bakeNavigation();
    navtime=0.001;
    
    if(Scene::common){
        for(int i=0;i<common->entities.size();i++){
            common->entities[i]->init();
        }
    }
    for(int i=0;i<entities.size();i++){
        entities[i]->init();
    }
    firstLoadScene=false;
}
Scene * Scene::Play()
{
    if(swap){delete swap;swap=0;}
    swap=new Scene(name);
    swap->setActive(); // for the sake of rigidbodies
    for(int i=0;i<entities.size();i++){
        Entity* tent=entities[i];
        bool isinst=tent->source.length();
        if(!tent->parent)tent->clone(true,isinst); // entities with parents are initiated by their parents
    }
    swap->setActive();
    // for the sake of camera
    if(swap->camera){Camera::active_camera=swap->camera;}
    else if(common&&common->camera){Camera::active_camera=common->camera;}
    else{Camera::active_camera=Camera::default_camera;}

    lua_State* L=Script::L;
    lua_getglobal(L,"Time");
       lua_getfield(L,-1,"timescale");
       Input::timescale=lua_tonumber(L,-1);
       lua_pop(L,1);
    lua_pop(L,1);
    return swap;
}

// setting scene during playtime using scripts
bool Scene::setScene(char type, int num,bool treload,std::string name)
{
    int scnum=-1;bool b=true;reload=treload;
    if(type==4){
        //search by name
        for(int i=0;i<ascenes.size();i++){
            if(ascenes[i]->name==name){scnum=i;break;}
        }
        if(scnum>-1){
            next=ascenes[scnum];
        }
        return b;
    }
    for(int i=0;i<ascenes.size();i++){
        if(ascenes[i]->swap==Scene::activeScene){scnum=i;break;}
    }
    bool np=scnum>-1&&scnum<ascenes.size()-1; // 0 to n-1
    bool pp=scnum>0&&scnum<ascenes.size();    // 1 to n
    if(type==0)         {next=Scene::findScene(Scene::activeScene->name);reload=true;}// reload
    else if(type==1&&np){next=ascenes[scnum+1];}//nexr scene
    else if(type==2&&pp){next=ascenes[scnum-1];}//prev scene
    else if(type==3){                                        //scene using index
         np=(num>-1&&num<ascenes.size());
         if(np){next=ascenes[num];}
    }
    else{b=false;}
    return b;
}

Entity* Scene::PickEntity(){
    Entity* sel=0;vec3 ray=Camera::active_camera->getCamRay();
    vec3 ip;vec3 cp=Camera::active_camera->pos;

    Camera* camera=Camera::active_camera;
    if(camera->ortho){
        vec2 mp=Shader::scrMousePos();
        mp.x/=Shader::sc_res.x/2.0;
        mp.y/=Shader::sc_res.y/2.0;
        vec3 cd=camera->dir;vec3 up=vec3(0.0,1.0,0.0);
        vec3 right=cd.Cross(up);up=right.Cross(cd);
        cp+=right*camera->aspect_ratio*camera->size*mp.x;
        cp+=up*camera->size*mp.y;
        ray=camera->dir;
    }


    vec3 pp=cp+vec3(999.0,999.0,999.0);
    for(int i=0;i<Scene::activeScene->entities.size();i++){
        Entity *e=Scene::activeScene->entities[i];Box*box;
        if(e->mesh&&e->isEnabled()){
            box=&e->mesh->box;
            box->tick(e->rtrans);
            if(box->IntersectRay(ray,cp,ip)&& ray.Dot(ip-cp)>0 && (cp.SqDistance(ip)<cp.SqDistance(pp))){
                pp=ip;
                sel=e;
            }
        }
        // TEMPORARY select scripts using mouse
        else if(e->script||e->waypoint){
            Box tbox(vec3(-1.0),vec3(1.0));box=&tbox;
            box->tick(e->rtrans);
            if(box->IntersectRay(ray,cp,ip) && (cp.SqDistance(ip)<cp.SqDistance(pp))){
                pp=ip;
                sel=e;
            }
        }
    }
    return sel;
}

void Scene::refresh(){
    vec3 cp=Camera::active_camera->pos;
    vec3 cd=Camera::active_camera->dir;
    float angle=(90.0f)*M_PI/180.0f;
    float asp=9.0/16.0;
    float radius=Light::dirlradius*2;
    vec3 centre=cp+cd*radius;
    float bounds[6]={radius,radius,radius,radius,radius,radius};

    vec3 dirs[5];
    vec3 right=cd.Cross(vec3::Yaxis()).Normalize(),up=right.Cross(cd).Normalize();
    dirs[0]=quat::Rotate(quat::Rotation(angle,up),right);
    dirs[1]=quat::Rotate(quat::Rotation(-angle,up),-right);
    dirs[2]=quat::Rotate(quat::Rotation(angle*asp,right),-up);
    dirs[3]=quat::Rotate(quat::Rotation(-angle*asp,right),up);
    dirs[4]=cd;
    Light* light=0;
    for(int i=0;i<entities.size();i++){
        if(entities[i]->light&&entities[i]->light->type==T_DIRECT&&entities[i]->light->shadow)
        {light=entities[i]->light;break;}
    }
    for(int i=0;i<entities.size();i++){
        Entity* go=entities[i];
        go->renderFlags=0;
        if(go->mesh){
            Box& box= go->mesh->box;
            box.tick(go->rtrans);
            for(int u=0;u<5;u++){
                if(!box.IntersectPlane(cp,dirs[u]))
                    {go->renderFlags=go->renderFlags|NORENDER;break;}//entities out of frustum
            }
            vec3 tvec;
            if(go->renderFlags&NORENDER && box.IntersectRay(cd,cp,tvec))//frustum fully inside frustum
            {go->renderFlags=go->renderFlags^NORENDER;}
        }
        if(!light||!light->shadow)go->renderFlags=go->renderFlags|NOSHADOW;
    }
    if(light){
        light->doShadow(centre,bounds);
    }
    /*
    Entity *plight=0;
    for(int i=0;i<entities.size();i++){
        if(entities[i]->light&&entities[i]->light->type==T_POINT){
            plight=entities[i];break;
        }
    }
    if(plight){
        plight->light->doCubeShadow();
    }
    */

}
void Scene::render()
{
//     if(!play)
        navigate();
    std::vector<Entity*> trentities;
    trentities.reserve(entities.size());
    vec3 cp=Camera::active_camera->pos;
    // safety measure
    bool einf=false;//entity at infinity
    for(int i=0;i<entities.size();i++){
        vec3 cp=entities[i]->rtrans.position;
       if(isnan(cp.x)||isnan(cp.y)||isnan(cp.z)){einf=true;break;}
    }
    cp=Camera::active_camera->pos;
    if(einf||isnan(cp.x)||isnan(cp.y)||isnan(cp.z)){
        for(int i=0;i<entities.size();i++){
            if((entities[i]->renderFlags&NORENDER)==0){
                printf("cpos =(%f,%f,%f) \n",cp.x,cp.y,cp.z);
                float d=(cp-entities[i]->rtrans.position).SqMagnitude();
                printf("dist =%f \n",d);
                if(entities[i]->mesh)
                    entities[i]->render();
            }
        }
        return;
    }
    // end safety
    
    for(int i=0;i<entities.size();i++){
        if((entities[i]->renderFlags&NORENDER)==0){
            if(entities[i]->mesh&&(entities[i]->mesh->flags&TRANSPARENT)==0)
                entities[i]->render();
            else
                trentities.push_back(entities[i]);
        }
    }
    int dists[trentities.size()];
    Entity* trents[trentities.size()];
    for(int i=0;i<trentities.size();i++){
        float d=(cp-trentities[i]->rtrans.position).SqMagnitude();
        int rank=0;
        for(int j=0;j<trentities.size();j++){
            float td=(cp-trentities[j]->rtrans.position).SqMagnitude();
            if(td>d||(td==d && i<j))rank++;
        }
        dists[i]=rank;
    }
    for(int i=0;i<trentities.size();i++){
        trents[dists[i]]=trentities[i];
    }
    for(int i=0;i<trentities.size();i++){
        trents[i]->render();
    }
}

std::string Scene::newname(std::string tname)
{
    int reps=0;char fbuff[8];
    std::string ttname=tname;
    /*
    for(int i=0;i<entities.size();i++){
        if(tname==entities[i]->name.substr(0,tname.length())){reps++;}
    }
    if (reps){
        snprintf(fbuff, sizeof(fbuff), "(%d)",reps-1);
        ttname=ttname+fbuff;
    }
    */
    return ttname;
}
bool raycastbool(vec3 v1,vec3 v2){
    btCollisionWorld::ClosestRayResultCallback	callback(toBVec3(vec3()),toBVec3(vec3(1.0)));
    callback.m_flags|=1<<0;
    Entity::dynamicsWorld->rayTest(toBVec3(v1),toBVec3(v2),callback);
    bool hit=callback.hasHit();
    return hit;
}
bool Araycastbool(vec3 v1,vec3 v2){
    btCollisionWorld::AllHitsRayResultCallback allResults(toBVec3(v1),toBVec3(v2));
    Entity::dynamicsWorld->rayTest(toBVec3(v1),toBVec3(v2),allResults);
    bool hit=false;
    for (int i=0;i<allResults.m_collisionObjects.size();i++)
    {
        if((allResults.m_collisionObjects[i])->isStaticObject())
        return true;
//         {hit= true;}
    }
    return hit;
}
void Scene::bakeNavigation()
{
    printf("@@@@@@@@ Baking Navigation \n");
    Entity::dynamicsWorld->updateAabbs();
    waypoints.clear();
    waypoints.reserve(entities.size());
    for(int i=0;i<entities.size();i++){
        Entity* e=entities[i];
        if (e->waypoint){
            e->waypoint->neighbours.clear();
            e->waypoint->neighbours.reserve(entities.size());
            waypoints.push_back(e);
        }
    }
    int numwp=waypoints.size();
    for(int i=0;i<numwp-1;i++){
        Entity* e=waypoints[i];
            for(int j=i+1;j<numwp;j++){
                Entity* te=waypoints[j];
                if (te->waypoint){
                    if(!Araycastbool(e->rtrans.position,te->rtrans.position)){
                        e->waypoint->neighbours.push_back(te);
                        te->waypoint->neighbours.push_back(e);
                    }
                }
            }
    }
    
}
bool wpdraw=false;
void Scene::navigate()
{
    if(Input::getKeyDown(KEY_F))wpdraw=!wpdraw;
    navtime-=Input::deltaTime;
    if(navtime<0){bakeNavigation();navtime=1000.8;}
    if(wpdraw||!Scene::play){
     for(int i=0;i<entities.size();i++){
        Entity* e=entities[i];
        Waypoint* wp=e->waypoint;
        if (wp){
            vec3 colour(sin(Input::time),cos(Input::time),0);
            Shader::renderPrim(0,e->rtrans,vec4(colour,1.0));
            for(int j=0;j<wp->neighbours.size();j++){
                Shader::drawLine(e->rtrans.position,wp->neighbours[j]->rtrans.position,vec3(1.0,1.0,0));
            }
        }
    }
    }
}

void rearrange(std::vector<Entity*>& stack,Entity* newb){
    int ssize=stack.size();
    for (int i=0;i<ssize;i++){
        if(newb->waypoint->tcost<stack[i]->waypoint->tcost)
        {stack.insert(stack.begin()+i,newb);/*printf("%d added %s to stack \n",i,newb->name.c_str());*/return;}
    }
//     printf("added %s to stack \n",newb->name.c_str());
    stack.push_back(newb);
}
bool vremove(std::vector<Entity*>& vec,Entity* obj){
    typename std::vector<Entity*>::const_iterator iter;
    for(iter = vec.begin(); iter != vec.end(); ++iter )
    {
      if( *iter == obj )
      {
	  vec.erase( iter );
	  return 1;
	  break;
      }
    }
    return 0;
}
bool vexists(std::vector<Entity*>& vec,Entity* ent){
    for(int i=0;i<vec.size();i++){
        if(ent==vec[i])return true;
    }
    return false;
}

bool Scene::getPath(vec3 v1,vec3 v2,std::vector<vec3>&points){
    float dist1=FLT_MAX;float dist2=dist1;
    // if not obstructed
    
    if(!Araycastbool(v1,v2)){points.push_back(v2); Shader::drawLine(v1,v2,vec3(0.6,1.0,2));return true;}
    int numwp=waypoints.size();
   
    Entity* snode=new Entity("tsnode"),*enode=new Entity("tenode");
    snode->transform.position=v1;enode->transform.position=v2;
    snode->waypoint=new Waypoint();enode->waypoint=new Waypoint();
    Scene::activeScene->entities.push_back(snode);Scene::activeScene->entities.push_back(enode);
    enode->transform.update();snode->transform.update();
    snode->rtrans=snode->transform;enode->rtrans=enode->transform;
    bakeNavigation();
//     navigate();
    if(!snode->waypoint->neighbours.size()||!enode->waypoint->neighbours.size()){return false;}
        
    for(int i=0;i<numwp;i++){
        Entity* e=waypoints[i];
//         float tdist1=e->rtrans.position.SqDistance(v1);
        float tdist2=e->rtrans.position.SqDistance(v2);
        e->waypoint->gcost=0;e->waypoint->fcost=tdist2;
    }

    
    std::vector<Entity*> finished,stack;
    stack.push_back(snode);
    Entity* cnode=snode;
    int attempts=numwp*numwp;
    while(stack[0]!=enode &&attempts){
        attempts--;
        cnode=stack[0];
        vremove(stack,cnode);
        finished.push_back(cnode);
//         printf("|cnode =%s| \n",cnode->name.c_str());
        for(int i=0;i<cnode->waypoint->neighbours.size();i++){
            Entity* tnode=cnode->waypoint->neighbours[i];
            tnode->waypoint->gcost=cnode->waypoint->gcost+tnode->rtrans.position.SqDistance(cnode->rtrans.position);
            tnode->waypoint->tcost=tnode->waypoint->fcost+tnode->waypoint->gcost;
            bool isfinished=vexists(finished,tnode);bool instack=vexists(stack,tnode);
            if(!isfinished&&!instack){
                tnode->waypoint->from=cnode;
                rearrange(stack,tnode);
            }
            // following is not necessary since gcost is distance and closest path is straight line :)
            /*
            else if(instack && 
            cnode->waypoint->gcost+cnode->rtrans.position.SqDistance(tnode->rtrans.position)<tnode->waypoint->gcost){
                tnode->waypoint->from=cnode;
                vremove(stack,tnode);
                rearrange(stack,tnode);
            }*/
//             printf("    stack: \n");
//             for(int i=0;i<stack.size();i++){
//                 printf("        %s\n",stack[i]->name.c_str());
//             }
            
        }
//         printf("------finished: \n");
//         for(int i=0;i<finished.size();i++){
//             printf("%s\n",finished[i]->name.c_str());
//         }
    
    }
    if(!attempts){
        Entity::remove(snode); 
        Entity::remove(enode);
        printf("Failed to find path  :(   !!!!!");
        return false;
    }
    attempts=numwp;
    Entity* tn=enode;
    printf("path ::\n");
    std::vector<vec3> revpoints;
    while(tn!=snode && attempts){
            printf("%s -> ",tn->name.c_str());
            attempts--;
            revpoints.push_back(tn->rtrans.position);
            tn=tn->waypoint->from;
    }
    points.clear();
    int numpoints=revpoints.size();
    for(int i=0;i<numpoints;i++){
        points.push_back(revpoints[numpoints-1-i]);
    }
    printf("%s :)\n",snode->name.c_str());
    /*
    vec3 off=vec3(0,1,0);
    vec3 tv1=snode->rtrans.position+off,tv2=points[points.size()-1]+off;
     Shader::drawLine(tv1,tv2,vec3(1.0));
    for(int i=0;i<points.size()-1;i++){
        vec3 off=vec3(0,1,0);vec3 tv1=points[i]+off,tv2=points[i+1]+off;
        Shader::drawLine(tv1,tv2,vec3(1.0));
    }
    */
    Entity::remove(snode); Entity::remove(enode);
    if(!attempts){printf("Failed to find starting node  :(  ): !!!!!");return false;}
    return true;
    
}
    

