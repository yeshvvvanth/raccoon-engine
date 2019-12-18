#include"manup.h"
#include"Editor.h"

vec3 Manupulator::scup,Manupulator::scright;
Manupulator::Manupulator()
{
    trans=RawMesh::findRawMesh("primitives#translate",1);
    rot=RawMesh::findRawMesh("primitives#rotate",1);
    scale=RawMesh::findRawMesh("primitives#scale",1);
    manup=trans;local=false;
}
Entity* Entity::isInst(Entity* te){
    if(te->source.length())return te;
    else if(te->parent) return isInst(te->parent);
    return 0;
}
bool movemode=false;
void Manupulator::tick()
{
    Camera* camera=Camera::default_camera;
    EMenu* emenu=EMenu::emenu;
    bool nty=!Input::typing;
    manup=trans;
    if(Input::getKey(KEY_LSHIFT)&&!(Input::getMouseButton(1)))
    {manup=scale;}
    else if(Input::getKey(KEY_LCTRL)&&!(Input::getMouseButton(1)))
    { manup=rot;}
    Entity* ae=EMenu::activeEntity;
    if(Input::getKeyDown(KEY_R)&&!Input::typing)movemode=!movemode;
    bool tilemapping=ae&&ae->gridmap;
    // MANUPULATOR 
    vec3 ray=camera->getCamRay(),cp=camera->pos;
    bool dr=!(Input::getKey(KEY_V)||Input::getKey(KEY_B));
    bool mandrag=false;
    vec3 cpt[3];
    vec3 axis;
    Entity* top=0;
    if(nty&&Input::getKeyDown(KEY_L)){local=!local;}
    if(movemode && ae){
        Transform ttrr;
        ttrr.position=ae->rtrans.position;ttrr.scale=vec3(0.1*(ttrr.position-camera->pos).Magnitude());
        ttrr.update();
        Shader::renderPrim(0,ttrr,vec4(1.0));
    }
    if(!movemode&&ae&& dr&&!ae->gridmap){
        float r= camera->pos.Distance(ae->rtrans.position)*0.024f;
        for(int i=0;i<3;i++){
            vec3 taxis;taxis[i]=1.0;
            if(local)taxis=quat::Rotate(ae->rtrans.rotation,taxis);
            if(rayAxis(ray,cp,r,taxis,cpt[i])){
                if(Input::getMouseButtonDown(0) ){
                    a=i;emenu->dragged=true;mandrag=true;
                }
            }
        }
        if(!Input::getMouseButton(0)){emenu->dragged=false;}
        if( emenu->dragged)
        {
            axis[a]=1.0f;
            if(local)axis=quat::Rotate(ae->rtrans.rotation,axis);
            
            for(int i=0;i<emenu->selected.size();i++){
                Entity *se=((ObjButton<Entity>*)emenu->selected[i])->object;
                vec3 diff=(cpt[a]-pcpt[a]);
                vec3 dd=(axis *axis.Dot(diff));
                
                if(manup==trans){
                    se->rtrans.position +=dd;
                    if(se->waypoint && dd.SqMagnitude()>0.00001)
                    {Scene::activeScene->bakeNavigation();}
                }
                else if(manup==scale && !emenu->activeEntity->source.length()){
                    if(!Input::getKey(KEY_LALT))
                    dd=quat::Rotate(se->rtrans.rotation.inverse(),dd);
                    se->rtrans.scale =dd+se->rtrans.scale ;
                }
                else if(manup==rot)
                    se->rtrans.rotation = quat::FromEulerAngles(dd*0.2)*se->rtrans.rotation;

                se->rtrans.update();
                se->transform=se->ptrans.inverse()*se->rtrans;
                se->update(); 
            }
        }
        for(int i=0;i<3;i++){
            pcpt[i]=cpt[i];
        }
    }
    
    if(!movemode&& dr&&!emenu->dragged && Shader::scrMousePos().x<emenu->inspector->x && 
        Shader::scrMousePos().x>emenu->x+emenu->width&&Input::getMouseButtonDown(0)&&!tilemapping )
    {
        printf("tried to select\n");
        Entity *sel=Scene::PickEntity();
        if(sel&& EMenu::addable(sel)){    // don't select children of gridmap
            printf(" active scene =%s \n",Scene::activeScene->name.c_str());
            printf(" selected |%s| \n\n",sel->name.c_str());
            top=Entity::isInst(sel);
            if(top)
                emenu->activeEntity=top;
            else
                emenu->activeEntity=sel;
            for(int a=0;a<emenu->tbuttons.size();a++){
                if(((ObjButton<Entity>*)emenu->tbuttons[a])->object==emenu->activeEntity)
                {emenu->active=emenu->tbuttons[a];emenu->tbuttons[a]->toggle=true;}
            }
            emenu->inspector->refresh();
            if(!Input::getKey(KEY_LCTRL)&&!Input::getKey(KEY_LSHIFT)){emenu->selected.clear();}
            emenu->selected.push_back(emenu->active);
        }
    }
    if(!EMenu::activeEntity){return;}
     // move without manupulator
    if(movemode&&!mandrag&&Input::getMouseButton(0)&&ae){
        vec2 md=Input::getMousedelta();
        vec3 right= quat::Rotate(camera->rot.inverse(),vec3::Xaxis());
        vec3 up= quat::Rotate(camera->rot.inverse(),-vec3::Yaxis());
        float mag=0.1;
        if(Input::getKey(KEY_LSHIFT)){
            ae->rtrans.scale += vec3((md.x-md.y)*0.01);
        }
        else{
            ae->rtrans.position+=up*md.y*mag+right*md.x*mag;
        }
        
        ae->rtrans.update();
        ae->transform=ae->ptrans.inverse()*ae->rtrans;
        ae->update(); 
        
        emenu->dragged=true;
    }
    /*
    if(Input::getMouseButton(0)&&!mandrag){
        emenu->dragged=true;
        Entity* se=EMenu::activeEntity;
        vec2 md=Input::getMousedelta();
        float mag=(md.x+md.y);
        mag*=(se->rtrans.position-Camera::active_camera->pos).Magnitude();
        
        if(Input::getKey(KEY_LSHIFT)){
            se->rtrans.scale+=vec3(1+0.2*mag);
        }
        else if(Input::getKey(KEY_LCTRL)){
            se->rtrans.rotation = quat(scright,scup+(scup-scright)*mag*0.00000001)*se->rtrans.rotation;
        }
        else{
        }
        se->rtrans.position +=(scright*md.x+scup*md.y)*0.08;
        se->rtrans.update();
        se->transform=se->ptrans.inverse()*se->rtrans;
        se->update(); 
    }
    */
    
    // Drawing the manupulator itself
    if(ae&&!ae->gridmap){
        Shader::simple->use();
        glBindVertexArray(manup->vao);
        Transform tr;quat mrot;
         glDisable(GL_DEPTH_TEST);
        
        if(EMenu::activeEntity){
            tr.position=EMenu::activeEntity->rtrans.position;
            tr.scale=(camera->pos.Distance(EMenu::activeEntity->rtrans.position)/15.0f);
            tr.update();
            if(local)mrot=EMenu::activeEntity->rtrans.rotation;
        }
        mat4 mvp=camera->proview*tr.model;
            
        vec3 axies[3];axies[0][1]=PI/2.0;axies[1][0]=-PI/2.0;axies[2][2]=-PI/2.0;
        for(int k=0;k<3;k++){
            vec4 colour;colour[k]=colour[3]=1.0;
            if(k==a&&emenu->dragged)colour=vec4(1.0,1.0,0.0,1.0); // set yellow if dragged
            glUniform4fv(glGetUniformLocation(Shader::simple->PID, "color"),1,colour.elements);
            tr.rotation=mrot*quat::FromEulerAngles(axies[k]);tr.update();mvp=camera->proview*tr.model;
            glUniformMatrix4fv(Shader::simple->uniMVP, 1, GL_FALSE, (mvp.elements));
            glDrawElements(GL_TRIANGLES,manup->es , GL_UNSIGNED_SHORT, 0);
        }
        glEnable(GL_DEPTH_TEST);
    }
        
     // quick rotate    
     if(nty&&(Input::getKeyDown(KEY_X)||Input::getKeyDown(KEY_Y)||Input::getKeyDown(KEY_Z))){
       Entity* tent=EMenu::activeEntity;
       vec3 angle;
       if(Input::getKeyDown(KEY_Y))angle[1]=PI/2.0;
       else if(Input::getKeyDown(KEY_X))angle[0]=PI/2.0;
       else angle[2]=PI/2.0;
       if(Input::getKey(KEY_LSHIFT))angle=-angle;
       if(tent){
            tent->rtrans.rotation=quat::FromEulerAngles(angle)* tent->transform.rotation;
            tent->rtrans.update();
            tent->transform=tent->ptrans.inverse()*tent->rtrans;
            tent->update();
            
            if(tent->rigidBody){
                btTransform btr(toBQuat(tent->rtrans.rotation),toBVec3(tent->rtrans.position));
                tent->rigidBody->setCenterOfMassTransform(btr);
            }
            EMenu::emenu->dragged=true;
       }
       
    }
    if(Input::getKeyDown(KEY_LSHIFT,KEY_X)){
       Entity* tent=EMenu::activeEntity;
       if(tent){
            tent->rtrans.rotation=quat();tent->rtrans.scale=vec3(1.0);
            tent->rtrans.update();
            tent->transform=tent->ptrans.inverse()*tent->rtrans;
            tent->update();
           if(tent->rigidBody){
                btTransform btr(btQuaternion(0,0,0,1),btVector3(0,0,0));
                tent->rigidBody->setCenterOfMassTransform(btr);
           }
           EMenu::emenu->dragged=true;
       }
    }

    if(nty&&Input::getKeyDown(KEY_H)){
       Entity* tent=EMenu::activeEntity;
       if(tent){
            if(Input::getKey(KEY_LSHIFT)){
                for(int i=0;i<Scene::activeScene->entities.size();i++){
                    Entity* ent=Scene::activeScene->entities[i];
                    ent->enabled=true;
                }
            }
            else if(Input::getKey(KEY_LCTRL)){
                tent->enabled=true;
            }
            else{
                tent->enabled=false;
            }
       } 
        EMenu::emenu->dragged=true;
    }
}
bool  Manupulator::rayAxis(vec3 ray,vec3 cp,float r,vec3 axis,vec3& ip){
    
    vec3 pos=EMenu::activeEntity->rtrans.position;
    vec3 n2= axis.Cross( ray.Cross(axis) );
    vec3 cpt=cp+( (pos-cp).Dot(n2)/ray.Dot(n2) )*ray ;  // closest point to cp on the axis
    
    vec3 cop=ray.Cross(axis);
    float ax= abs( cop.Dot(pos-cp) )/cop.Magnitude();
    
    ip=cpt;
    float tcd=(Camera::default_camera->pos.Distance(pos)/5.8f); //6.0 to be exact
    float tdd=axis.Dot(cpt-pos);
    if( (ax<r) && ((tdd>0&& tdd < tcd)) ){;return true;}
    else return false;
}
