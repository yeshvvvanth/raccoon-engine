#include"waypoint.h"
#include"entity.h"
#include"../main/scene.h"
Waypoint::Waypoint():fcost(0.0f),gcost(0.0f),tcost(0.0f){
    entity=from=0;
}
#define TINMAX 255

GridMap::GridMap(int tsx,int tsy){
    sizex=tsx;sizey=tsy;
    dimensions=vec3(12.0);
    tiles=new unsigned char[sizex*sizey*2];
    memset(tiles,TINMAX,sizex*sizey);
    memset(tiles+sizey*sizex,0,sizey*sizex);
    active=0;snames.clear();
}

GridMap::~GridMap()
{
    delete []tiles;
}

void getSnames(std::vector<std::string>& snames){
    snames.clear();
    Entity *pretile=Entity::findEntity("tiles",1);
    if(pretile){
        for(int i=0;i<pretile->children.size();i++){
            snames.push_back(pretile->children[i]->name);
        }
    }
}
void getElist(std::vector<std::string>& snames,std::vector<Entity*> &elist){
    elist.clear();
    for(int i=0;i<snames.size();i++){
        Entity *te=Entity::findEntity(snames[i],1);
        elist.push_back(te);
    }
}
void resort(GridMap* gmap);
void Instantiate(Entity* ent,GridMap* gmap);

void GridMap::refresh(Entity* ent){
    if(snames.size()){
        if(!Scene::play)resort(this);
        Instantiate(ent,this);
    }else{
        getSnames(snames);
    }
}
void GridMap::clean(Entity* ent)
{
      int chsize=ent->children.size();
    for(int i=0;i<chsize;i++){
        Entity::remove(ent->children[chsize-i-1]);          // remove children from last to first 
    }
}

void Instantiate(Entity* ent,GridMap* gmap)
{
    GridMap::clean(ent);
    std::vector<Entity*> elist;
    getElist(gmap->snames,elist);
    vec3 Xaxis(1,0,0),Yaxis(0,1,0),Zaxis(0,0,1);
    unsigned char f=0;float g2x=float(gmap->sizex)/2.0,g2y=float(gmap->sizey)/2.0;
    for(int i=0;i<gmap->sizex;i++){
        for(int j=0;j<gmap->sizey;j++){
            unsigned char& tind=gmap->tiles[i*gmap->sizex+j];
            unsigned char &trot=gmap->tiles[i*gmap->sizex+j+(gmap->sizex*gmap->sizey)];
            if(tind<TINMAX&&elist[tind]){
                Entity* te=elist[tind]->clone(false,true);te->parent=0;
                te->source=elist[tind]->name;           // to avoiding deletion of collision shape
                ent->update();
                te->transform.position=ent->rtrans.position+
                                      gmap->dimensions.x*(i-g2x)*Xaxis+
                                      gmap->dimensions.z*(j-g2y)*Zaxis+
                                      gmap->dimensions.y*f*Yaxis;
                te->transform.rotation=quat::FromEulerAngles(vec3(0,PI/4.0*trot,0));
                te->transform.update();te->update();
                ent->addChild(te);
            }
        }
    }
}
unsigned char getNewSname(std::string tname,std::vector<std::string> &newsnames){
    unsigned char tc=TINMAX;
    for( int i=0;i<newsnames.size();i++){
        if(tname==newsnames[i]){tc=i;break;}
    }
    return tc;
}
void resort(GridMap* gmap)
{
    std::vector<unsigned char> newInds;
    std::vector<std::string> newsnames;
    getSnames(newsnames);
    for(int i=0;i<gmap->snames.size();i++){
        unsigned char tind=getNewSname(gmap->snames[i],newsnames);
        newInds.push_back(tind);
    }
    gmap->snames=newsnames;
    
    for(int i=0;i<gmap->sizex;i++){
        for(int j=0;j<gmap->sizey;j++){
            unsigned char& tind=gmap->tiles[i*gmap->sizex+j];
            if(tind!=TINMAX){
                tind=newInds[tind];
                if(tind>newInds.size())printf("something went wrong!!! tind=%d \n",tind);
            }
        }
    }
    
}


    
