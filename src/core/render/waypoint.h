#pragma once
#include<vector>
#include<string>
#include"../maths/maths.h"
struct Entity;
struct Waypoint{
    Entity* entity,*from;
    float gcost,fcost,tcost;
    std::vector<Entity*> neighbours;
    Waypoint();
};


struct GridMap{
    int sizex,sizey,floors;
    vec3 dimensions;
    unsigned char* tiles,active;
    void refresh(Entity* ent);
    std::vector<std::string> snames;
    static void clean(Entity* ent);
    GridMap(int sizex=8,int sizey=8);
    ~GridMap();
};
