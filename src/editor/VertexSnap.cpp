#include"Editor.h"
#include"manup.h"
vec3 v1,v2;
unsigned int rayVert(vec3 ray,vec3 cp,Entity* ent,vec3& rv,float& dis,bool);
void editTerrain();
void EditGridmap();
void VertexSnap(){
    Camera* camera=Camera::default_camera;
    EMenu* emenu=EMenu::emenu;
    vec3 ray=camera->getCamRay(),cp=camera->pos;
    EditGridmap();
    // MESH AABB 
    if(EMenu::activeEntity && EMenu::activeEntity->mesh){
        float talpha=Sprite::sp->alpha;
        Sprite::sp->alpha=1.0f;
        Entity* ae=EMenu::activeEntity;
        RawMesh* tm=RawMesh::findRawMesh("primitives#cube",1);
        tm->shader->use();
        
        for(int i=0;i<emenu->selected.size();i++){
            Entity* se=((ObjButton<Entity>*)emenu->selected[i])->object;
            if(se->mesh){
                Box &b=se->mesh->box;
                b.tick(se->rtrans);
            
                Transform tr;
                tr.position=(b.L+b.U)*0.5;
                tr.scale= (b.odx+b.ody+b.odz-(b.ll*3))*se->rtrans.scale*0.505*2.0;
                tr.rotation=se->rtrans.rotation;
                tr.update();
                
                mat4 mvp=camera->proview*tr.model;
                glUniformMatrix4fv(tm->shader->uniMVP,1,GL_FALSE,mvp.elements);
                vec4 tint(0.8,0.7,0.6,1.0);
                tm->shader->use();
                glUniform4fv(glGetUniformLocation(tm->shader->PID,"color"),1,tint.elements);
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                    tm->render(tr);
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            }
        }
        // VERTEX SNAPPING

        if(Input::getKey(KEY_V)){
            Sprite *sp=Sprite::sp;
            vec3 rv;float dis=9999;
            if(Input::pmbuttons[0]&& !Input::mbuttons[0]){
                emenu->dragged=true;
                for(int i=0;i<emenu->selected.size();i++){
                    Entity* se=((ObjButton<Entity>*)emenu->selected[i])->object;
                    vec3& pos=se->rtrans.position;
                    pos += v2-v1;
                    se->rtrans.update();
                    se->transform=se->ptrans.inverse()*se->rtrans;
                }
            }
            if(!Input::getMouseButton(0)){
                for(int i=0;i<emenu->selected.size();i++){
                    Entity* se=((ObjButton<Entity>*)emenu->selected[i])->object;
                    if(se->mesh)rayVert(ray,cp,se,rv,dis,Input::getKey(KEY_LCTRL));
                }
                if(dis!=9999){
                    dis=9999;
                    v1=rv;
                }
            }
            vec4 p=camera->proj*(camera->view*vec4(v1,1.0));
            p*=1/p.w;
            vec2 sr=Shader::getResolution();  
            sp->resize(p.x*sr.x*0.5-10.0,p.y*sr.y*0.5-10.0,20.0,20.0);
            sp->color=vec3(0.99,0.2,0.2);
            sp->tick();
            if(Input::getMouseButton(0)){
                Entity *tpe;float d1=dis;
                for(int p=0;p<Scene::activeScene->entities.size();p++){
                    Entity *te=Scene::activeScene->entities[p];
                    if(te->mesh && te!=ae){
                        rayVert(ray,cp,te,rv,dis,Input::getKey(KEY_LCTRL));
                        if(d1!=dis){d1=dis;tpe=te;}
                    }
                }
                v2=rv;
                p=camera->proj*(camera->view*vec4(v2,1.0));
                p*=1/p.w;
                sr=Shader::getResolution();  
                sp->resize(p.x*sr.x*0.5-10.0,p.y*sr.y*0.5-10.0,20.0,20.0);
                sp->color=vec3(0.9,0.9,0.2);
                sp->tick();
                if(tpe){
                    Box &b1=tpe->mesh->box;
                    b1.tick(tpe->rtrans);
                    Transform tr;
                    tr.position=(b1.L+b1.U)*0.5;
                    tr.scale= (b1.odx+b1.ody+b1.odz-(b1.ll*3))*tpe->transform.scale*0.505;
                    tr.rotation=tpe->transform.rotation;
                    tr.update();
                    
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                        tm->render(tr);
                    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                }
            }
        }
        Sprite::sp->alpha=talpha;
    }
    editTerrain();

}

// Intersection between ray and vertices of mesh or of its aabb
unsigned int rayVert(vec3 ray,vec3 cp,Entity* ent,vec3& rv,float& dis,bool pervert){
    unsigned int ind=0;
    // vertex snapping using mesh vertices 
    if(pervert && ent->mesh->type!=2){
        Mesh* mesh=(Mesh*)ent->mesh;
        for(short k=0;k<mesh->rawmesh->vs;k++){
            vec3 vert(mesh->rawmesh->vertices+k*6);
            vert=ent->rtrans.model*vert;
            float d=((vert-cp).Cross(ray)).SqMagnitude();
            if(d<dis){dis=d;rv=vert;ind=k;}
        }
    }
    // using aabb vertices
    else{
        Box &b=ent->mesh->box;
        b.tick(ent->rtrans);
        vec3 ves[8];short key=-1;
        vec3 dx=b.dx-b.L,dy=b.dy-b.L,dz=b.dz-b.L;
        ves[1]=b.dx;ves[2]=b.dy;ves[3]=b.dz;
        ves[4]=b.L+dx+dy;ves[5]=b.L+dy+dz;
        ves[6]=b.L+dz+dx;ves[7]=b.U;ves[0]=b.L;
        // loop through aabb vertices
        for(short k=0;k<8;k++){
            float d=((ves[k]-cp).Cross(ray)).SqMagnitude();
            if(d<dis){key=k;dis=d;rv=ves[key];}
        }        
    }
    dis=sqrt(dis);
    return ind;
}
void editTerrain()
{
    Entity* ae=EMenu::activeEntity;
    Camera* camera=Camera::default_camera;
    EMenu* emenu=EMenu::emenu;
    vec3 ray=camera->getCamRay(),cp=camera->pos,cv;float dis=999999;
    if(ae && ae->mesh&&ae->mesh->type==3){
        unsigned int ind=rayVert(ray,cp,ae,cv,dis,true);
        float* verts=((Terrain*)ae->mesh)->raw->vertices;
        Terrain* terrain=((Terrain*)ae->mesh);
        Transform tr;tr.position=vec3(verts+6*(ind));
        tr.scale=vec3(1,100.0,1);tr.update();
        Shader::renderPrim(0,tr,vec4(2.0));
        float dy=0.0;
        if(Input::getMouseButton(0))dy=0.2;
        if(Input::getMouseButton(2))dy=-0.2;
        if(ind>0&&ind<terrain->sizex*terrain->sizey&&dy){
            int h=int(ind/terrain->sizex),w=ind-(h*terrain->sizex);
            printf("ind =%d w=%d  h=%d  \n",ind,w,h);
            for(int i=0;i<terrain->sizex;i++){
                for(int j=0;j<terrain->sizey;j++){
                    float dist=sqrt((w-i)*(w-i)+(h-j)*(h-j));
                    float lift_radius=10.0;
                    float current_amount = (lift_radius - dist) / lift_radius;
                    current_amount = current_amount * current_amount * (3 - 2*current_amount); 
                    float fac=current_amount;
                    if(dist>lift_radius)fac=0;
                    if(fac<0)fac=0;
//                     float fac=1.0-dist;
//                     if(w==i&&h==j)fac=1.0;
                    float tf=verts[6*(j*terrain->sizex+i)+1]+=(dy*fac);
                    terrain->data[(j*terrain->sizex+i)]=short(tf*32.0);
                    if(tf>terrain->maxh)terrain->maxh=tf;
                    if(tf<terrain->minh)terrain->minh=tf;
                }
            }
            terrain->Refresh();
            if(ae->rigidBody){
                delete ae->rigidBody->getCollisionShape();
                ae->collShape=terrain->getCollider();
                ae->rigidBody->setCollisionShape(ae->collShape);
                
            }
        }
        
    }
    
}
int pp=255,pq=255;char tilerot=0;

void getDerivs(Entity* ent,std::vector<Entity*>& vec){
    vec.push_back(ent);
    for(int i=0;i<ent->children.size();i++){
        getDerivs(ent->children[i],vec);
    }
}

void EditGridmap()
{
    vec3 ray = Camera::active_camera->getCamRay();
    vec3 cp=Camera::active_camera->getCamPos();
    Entity* ae=EMenu::activeEntity;
    if(ae&& ae->gridmap){
        vec3 gpos=ae->rtrans.position;
        float d=((gpos.y-cp.y)/ray.y);
        GridMap* gm=ae->gridmap;
        vec3 ipos=cp+ray*d;vec3 &dim=gm->dimensions;
        vec3 xax=vec3(1,0,0);vec3 zax(0,0,1);
        vec3 xcol(0.6,0.8,1),ycol(0.4,0.6,0.8);
        vec3 rpos=ipos-gpos;
        int p=(rpos.x-dim.x)/dim.x,q=(rpos.z-dim.z)/dim.z;
        
        if(Input::getKeyDown(KEY_F)){
            tilerot+=2;
            if(Input::getKey(KEY_LSHIFT))tilerot--;
            if(tilerot>7)tilerot=0;
        }
        gm->active+=Input::scroll;
        if(gm->active>=0&&gm->active+Input::scroll<0)gm->active=gm->snames.size()-1;
        if(gm->active>gm->snames.size()-1)gm->active=0;
        EditorUI::actile->tick();
        if(Input::scroll&&gm->snames.size()){EditorUI::actile->setText(gm->snames[gm->active]);printf("actile =%s \n",gm->snames[gm->active].c_str());}
        p=(rpos.x-dim.x/2.0)/dim.x+float(gm->sizex)/2.0+1;q=(rpos.z-dim.z/2.0)/dim.z+float(gm->sizey)/2.0+1;
        bool valid=p>-1&&q>-1&&p<gm->sizex&& q<gm->sizey;
        bool clicked=(Input::getMouseButton(0)&&(p!=pp||q!=pq))||Input::getMouseButtonDown(0);
        bool clean=Input::getKey(KEY_LCTRL)||Input::getKey(KEY_LSHIFT);
        pp=p;pq=q;
        vec3 col(1.0);
        if(Input::getMouseButton(0))col=vec3(0,1,0);
        if((clicked) && gm->snames.size() && valid){
            unsigned char &tind=gm->tiles[p*gm->sizex+q];
            unsigned char &trot=gm->tiles[p*gm->sizex+q+(gm->sizex*gm->sizey)];
            bool refr=true;
            if(!clean){if(tind==gm->active&&trot==tilerot){refr=false;}tind=gm->active;trot=tilerot;}
            else{
                tind=255;trot=0;
            }
            printf("tind =%d active=%d\n",tind,gm->active);
            if(refr){
                gm->refresh(ae);
            }
//             EMenu::emenu->reset();
//             EMenu::emenu->inspector->refresh();
        }
        /// render stuff
        float g2x=float(gm->sizex)/2.0,g2y=float(gm->sizey)/2.0;
        Transform ttr;ttr.position=ipos;ttr.update();
        Shader::renderPrim(1,ttr,vec4(1));
        //test draw
        if(gm->active<gm->snames.size()&&!clean){
            ttr.position=gpos+(p-g2x)*dim.x*xax+(q-g2y)*dim.z*zax;
            Entity* rent=Entity::findEntity(gm->snames[gm->active],1);
            ttr.scale=rent->rtrans.scale;
            ttr.rotation=quat::FromEulerAngles(vec3(0,PI/4.0*tilerot,0));
            ttr.update();
            
            Transform tpt=rent->transform;
            Entity* tpr=rent->parent;
            rent->transform=ttr;
            rent->parent=0;
            
            std::vector<Entity*> renties;
            getDerivs(rent,renties);
            int rsize=renties.size();
            for(int i=0;i<rsize;i++){
                renties[i]->update();
            }
            
            for(int i=0;i<rsize;i++){
                renties[i]->render();
            }
            rent->parent=tpr;
            rent->transform=tpt;
                
            for(int i=0;i<rsize;i++){
                renties[i]->update();
            }
            /*
            if(rent&&rent->mesh&&rent->mesh->type==1){
                Mesh* tmesh=((Mesh*)rent->mesh);
                if(tmesh->texture)tmesh->texture->bind();
                tmesh->rawmesh->shader->use();
                mat4 mvp=Camera::active_camera->proview*ttr.model;
                glUniformMatrix4fv(tmesh->rawmesh->shader->uniMVP, 1, GL_FALSE,mvp.elements);
                tmesh->rawmesh->render(ttr);
                
                Shader::simple->use();
                mvp=Camera::active_camera->proview*ttr.model;
                glUniformMatrix4fv(tmesh->rawmesh->shader->uniMVP, 1, GL_FALSE,mvp.elements);
                glDrawElements(GL_LINES,tmesh->rawmesh->es, GL_UNSIGNED_SHORT, 0);
            }
            */
        }
        ttr.position=gpos+(p-g2x)*dim.x*xax+(q-g2y)*dim.z*zax+vec3(0,dim.x/2.0,0);
        ttr.scale=vec3(dim.x);
        ttr.update();
        
        if(clean)col=vec3(1.0,0,0);
        Shader::renderPrim(0,ttr,vec4(col,0.16));
            
        for(int i=0;i<gm->sizex;i++){
            vec3 sp=gpos+dim.x/2.0*xax+(i-(float(gm->sizex)/2.0))*dim.x*xax  -zax*gm->sizey*(dim.z/2.0);
            vec3 ep=sp+zax*(gm->sizey*dim.z);
            Shader::drawLine(sp,ep,ycol);
        }
        for(int j=0;j<gm->sizey;j++){
            vec3 sp=gpos+dim.z/2.0*zax+(j-(float(gm->sizey)/2.0))*dim.z*zax  -xax*gm->sizex*(dim.x/2.0);
            vec3 ep=sp+xax*(gm->sizex*dim.x);
            Shader::drawLine(sp,ep,xcol);
        }
        
    }
    
}


