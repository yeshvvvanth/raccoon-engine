#include"Editor.h"
void drawIcons(){
    
    Camera* camera=Camera::default_camera;
    EMenu* emenu=EMenu::emenu;
    vec3 ray=camera->getCamRay(),cp=camera->pos;
    Button* icon=Editor::icon;
    Shader* shader=Shader::findShader("simple");
    Entity* ae=EMenu::activeEntity;
    
    for(int i=0;i<Scene::activeScene->entities.size();i++){
        Entity* go=Scene::activeScene->entities[i];
        Texture *ct=Texture::findTexture("caicon.png",1),
        *st=Texture::findTexture("sicon.png",1),
        *lt=Texture::findTexture("licon.png",1);
        if(go->camera||go->light||go->sound)
        {
            vec3 v2=go->rtrans.position;
            bool b=((v2-cp).Dot(ray)>0);
            if(b)
            {
                icon->texture=go->light?lt:(go->sound?st:ct);
                
                vec4  p=camera->proview*vec4(v2.x,v2.y,v2.z,1.0);
                p*=1/p.w;
                vec2 sr=shader->getResolution();  
                float s=50.0;
                icon->resize(p.x*sr.x*0.5-s*0.5,p.y*sr.y*0.5-s*0.5,s,s);
                icon->color=vec3(0.9,0.9,0.2);
                icon->tick(); 
                icon->texture=0;
                if(icon->justDown){
                    Entity* top=Entity::isInst(go);
                    if (!top)top=go;
                    if((top&&top->parent&&top->parent->gridmap)||(ae&&ae->gridmap)){top=0;}      // don't select tilemapped entities
                    if(!Input::getKey(KEY_LCTRL)){emenu->selected.clear();}
                    for(int a=0;a<emenu->tbuttons.size();a++){
                        if(((ObjButton<Entity>*)emenu->tbuttons[a])->object==top)
                        {
                            emenu->activeEntity=top;
                            emenu->active=emenu->tbuttons[a];
                            emenu->selected.push_back(emenu->tbuttons[a]);
                            emenu->inspector->refresh();
                        }
                    }
                }
            }
            
        }
        if(!go->mesh&&go->script){
            Shader::renderPrim(2,go->rtrans,vec4(sin(Input::time*10),cos(Input::time*10),1.0,1.0));
        }
    }
    for(int a=0;a<emenu->tbuttons.size();a++){
        Entity* e=((ObjButton<Entity>*)emenu->tbuttons[a])->object;
        if(e->button&&e->button->down&&e!=emenu->activeEntity){
            emenu->selected.clear();
            emenu->activeEntity=e;
            emenu->active=emenu->tbuttons[a];
            emenu->selected.push_back(emenu->tbuttons[a]);
            emenu->inspector->refresh();
        }
    }
    if(emenu->activeEntity&& emenu->activeEntity->button&&Editor::showui){
        Button* but=emenu->activeEntity->button;
        vec2 md=Input::getMousedelta();
        vec2 delta=vec2(md.x,-md.y);
        vec2 pos=vec2(but->transform.position.x,but->transform.position.y);
        vec2 size=vec2(but->transform.scale.x,but->transform.scale.y);
        if(Input::getKey(KEY_LCTRL)&&!Input::getKey(KEY_LALT))
            pos=pos+delta;
        else if(Input::getKey(KEY_LSHIFT)&&!Input::getKey(KEY_LALT))
            size=size+delta;
        
        if(but->dragged){
            if(but->type==1||but->type==5){
                TextButton* tb=(TextButton*)but;
                tb->setPos(pos.x,pos.y,size.x,size.y);
            if(!Input::getKey(KEY_LSHIFT)&&Input::getKey(KEY_LALT))
            {tb->padX+=delta.x;tb->padY+=delta.y;}  
            if(Input::getKey(KEY_LSHIFT)&&Input::getKey(KEY_LALT))
            {tb->label->setScale(tb->label->scale+(delta.x+delta.y));}
            }
            else if(but->type==2){((Slider*)but)->setPos(pos.x,pos.y);;}
            else but->resize(pos.x,pos.y,size.x,size.y);
            emenu->dragged=true;printf("dragged!!!\n");
        }
        Sprite::render(0,0,480,270,vec3(0.0,0.0,0.1),0.4);
        Sprite::render(-480,-270,480,270*2,vec3(0.1,0.0,0.0),0.2);
        Sprite::render(-480,-270,480*2,270,vec3(0.0,0.1,0.0),0.2);
    }
}
