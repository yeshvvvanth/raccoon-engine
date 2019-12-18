#include"script.h"
#include "entity.h"
#include "../main/scene.h"
#define MAX_TABLE_REC 4
/// ///////////////////////////

ScriptProp::~ScriptProp(){}
std::string Script::source="";
lua_State* Script::L;
Script* Script::active=0;
Entity* Script::aentity=0;
std::vector<Script*> Script::scripts;
std::vector<std::string> Script::errors;
Sprite* Sprite::sp=0; 
bool Script::success=true;

void getind(ScriptProp* sp){
    if(sp->index>-1)
        lua_rawgeti(Script::L,-1,sp->index);
    else{
        std::string tname= sp->pub?"_"+sp->name:sp->name;
        lua_getfield(Script::L,-1,tname.c_str());
    }
}
void setind(ScriptProp* sp){
     if(sp->index>-1)
         lua_pushnumber(Script::L,sp->index);
     else{
         std::string tname= sp->pub?"_"+sp->name:sp->name;
         lua_pushstring(Script::L,tname.c_str());
     }
}

Script::Script():name("")
{
  entity=0;
}

Script::Script(std::string scname):name(scname)
{
  entity=0;
}
Script::~Script()
{
   for(int i=0;i<props.size();i++){delete props[i];}
   for(int i=0;i<p_props.size();i++){delete p_props[i];}
   props.clear();p_props.clear();
}


Script* Script::clone()
{
  Script* scr=new Script(this->name);
  // clone this one to scr 
  for(int i=0;i<props.size();i++)
    {scr->props.push_back(props[i]->clone());}
  for(int i=0;i<p_props.size();i++)
    {scr->p_props.push_back(p_props[i]->clone());}  
  return scr;
}
void Script::begin()
{
  L=luaL_newstate();
  luaL_openlibs(L);
  
  int ret=luaL_dostring(L,source.c_str());
  if(ret!=0){
     std::string err=lua_tostring(L,-1);
     err="Error :"+err;
     printf(" %s \n\n\n", err.c_str());
  }
  else{
        success=true;
  }
  errors.clear();
  registerClasses();
  //////////////
}
void Script::end()
{
  lua_close(L);
  for(int u=0;u<scripts.size();u++){
    delete scripts[u];  
    }
  scripts.clear();
}

void Script::registerScripts()
{
   if(success){
        lua_getglobal(L,"getKeys");
        lua_pcall(L,0,1,0);
        ///////////////////////////
        std::string test = lua_tostring(L, -1);
        
        std::string temp = "";
        for(unsigned int i = 0; i < test.size(); i++) 
        {     
            if(test.at(i) != ',')
            {
            temp += test.at(i);
            } 
            else
            {
            scripts.push_back(new Script(temp));
            temp= "";
            }
        }
        printf("scripts: %s \n",test.c_str());

        for(int i=0;i<scripts.size();i++){
            scripts[i]->loadScriptProps();
        }
   }

  
}

void Script::read(std::ifstream& ifs,bool eda,char* name)
{
    int size=0;
    char* data=0;
    ifs.read((char*)&size,4);
    data=new char[size];
    ifs.read((char*)data,size);

    if(eda){
        source.clear();
        source.assign(data,size);
    }
    else{
        if(luaL_loadbuffer(L,data,size,name)){// load chunk
            std::string err=lua_tostring(L,-1);
            printf("error: %s",err.c_str());
            err="Error :"+err;
            errors.push_back(err);
            lua_pop (L, -1);success=false;
        }
        if(lua_pcall(L, 0, 0, 0)){          // run chunk
            printf("error: %s", lua_tostring (L, -1));
            lua_pop (L, -1);success=false;
        }
        
    }
    delete []data;
}


///        ////////////////////////////////////////////
void Script::Init()
{
  active=this;
  setAEntity(this->entity);

  Update(1);
  
  lua_pushstring(L,name.c_str());
  lua_setglobal(L,"C_ascript");
  
  lua_getglobal(L,name.c_str());
  lua_pushstring(L,"Init");
  lua_gettable(L,-2);
  
  if(lua_isfunction(L,-1)){ 
    int ret=lua_pcall(L,0,0,0);
    if(ret!=0){
        printf("Error: %s \n\n\n", lua_tostring(L,-1));
        lua_pop(L,1);// pop the error out of stack
    }
  }  
  else
      lua_pop(L,1);

  lua_pop(L,1);
  Update(0);Update(2);
}

void Script::setAEntity(Entity* ent)
{
  aentity=ent;
  lua_getglobal(L,"entity");
  lua_pushstring(L,"udata");
  lua_pushlightuserdata(L,ent);
  lua_settable(L,-3);
  lua_pushstring(L,"name");
  lua_pushstring(L,ent->name.c_str());
  lua_settable(L,-3);
  lua_pop(L,1);
}


void Script::tick()
{
  active=this;
  setAEntity(this->entity);

  Update(1);Update(3);
  
  lua_pushstring(L,name.c_str());
  lua_setglobal(L,"C_ascript");

  lua_getglobal(L,name.c_str());
  lua_pushstring(L,"Update");
  lua_gettable(L,-2);
  if(lua_isfunction(L,-1)){ 
    int ret=lua_pcall(L,0,0,0);
    if(ret!=0){
        printf("Error: %s \n\n\n", lua_tostring(L,-1));
        lua_pop(L,1);// pop the error out of stack
    }
  }  
  else
      lua_pop(L,1);

  lua_pop(L,1);
  Update(0);Update(2);
}

void Script::Update(char type)
{
    lua_getglobal(L,name.c_str());
    if(type==0)
        for(int i=0;i<props.size();i++){props[i]->read(&name);}
    else if(type==1)
        for(int i=0;i<props.size();i++){props[i]->update(&name);}
    else if(type==2)
        for(int i=0;i<p_props.size();i++){p_props[i]->read(&name);}
    else if(type==3)
        for(int i=0;i<p_props.size();i++){p_props[i]->update(&name);}
    lua_pop(L,1);
    
}
/// SCRIPT PROPS ///////////////////////

void Script::loadScriptProps()
{
    lua_getglobal(L,name.c_str());
    
    lua_pushvalue(L, -1);
    // stack now contains: -1 => table
    lua_pushnil(L);
    // stack now contains: -1 => nil; -2 => table
    while (lua_next(L, -2))
    {
        // stack now contains: -1 => value; -2 => key; -3 => table
        // copy the key so that lua_tostring does not modify the original
        lua_pushvalue(L, -2);
        // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
        std:: string key = lua_tostring(L, -1);
        const char *value = lua_tostring(L, -2);
        
// 	printf("%s => %s\n\n\n", key.c_str(), value);
	///-- check if first character of the name of the variable is 'underscore'
    bool ppublic=key[0]=='_';
	if(key!="type"){
      std::string tkey=ppublic?key.substr(1,key.length()-1):key;
// 	  printf("++++++++++++++ name of var=%s +++++++++++\n\n",key.c_str());
	  if     (lua_isnumber (L,-2)){
// 	    printf("It is a number!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
	    ScriptProp* pr=new SFloat(tkey,ppublic);
	    if(ppublic)props.push_back(pr);else p_props.push_back(pr);
	    ((SFloat*)pr)->value=lua_tonumber(L,-2);
	  }
	  else if(lua_isboolean(L,-2)){
// 	    printf("It is a boolean!!!\n");
	    ScriptProp* pr=new Sbool(tkey,ppublic);
	    if(ppublic)props.push_back(pr);else p_props.push_back(pr);
	    ((Sbool*)pr)->value=lua_toboolean(L,-2);
	  }
	  else if(lua_isstring (L,-2)){
// 	    printf("It is a string!!!\n" );
	    ScriptProp* pr=new Sstring(tkey,ppublic);
	    if(ppublic)props.push_back(pr);else p_props.push_back(pr);
	    ((Sstring*)pr)->value=lua_tostring(L,-2);
	  }
      else if(lua_istable (L,-2)){
          int index=lua_gettop(L)-1;
          int type=0;
          lua_getfield(L,index,"type");
          if(lua_isnumber(L,-1)){
            type=lua_tonumber(L,-1);
            if(type==110){
                //get vector3
                printf("$$$$$$$$  VEC3 property name=%s\n",tkey.c_str());
                ScriptProp* pr=new SVec3(tkey,ppublic);
                if(0&&ppublic)props.push_back(pr);else p_props.push_back(pr);
                ((SVec3*)pr)->value=getvec3(L,index);
            }
            else if(type==112){
                //get quat
                ScriptProp* pr=new SQuat(tkey,ppublic);
                if(0&&ppublic)props.push_back(pr);else p_props.push_back(pr);
                ((SQuat*)pr)->value=getquat(L,index);
            }
          }
          else{
                lua_getglobal(L,name.c_str());
        	    printf("It is a table!!!\n" );
                printf("$$$$$$$$  table property name=%s\n",tkey.c_str());
        	    ScriptProp* pr=new STable(tkey);
        	    p_props.push_back(pr);// always private
        	    ((STable*)pr)->read(0);
                lua_pop(L,1);
                
          }
          
          lua_pop(L,1);
	  }
	  
	}
        // pop value + copy of key, leaving original key
        lua_pop(L, 2);
        // stack now contains: -1 => key; -2 => table
    }
    // stack now contains: -1 => table (when lua_next returns 0 it pops the key
    // but does not push anything.)
    // Pop table
    lua_pop(L, 2);
  
  ///////////////////
}

/// different types of script props

ScriptProp * Script::findProp(std::string sname)
{
    ScriptProp *sp=0;
    for(int q=0;q<props.size();q++){
        if(props[q]->name==sname){sp=props[q];break;}
    }
    
    return sp;
}


SFloat::SFloat(std::string name,bool tpub)
{
  this->name=name;index=-1;
  type=1;pub=tpub;
    value=420.0f;
}
Sbool::Sbool(std::string name,bool tpub)
{
  this->name=name;index=-1;
  type=2;pub=tpub;
    value=false;
}
Sstring::Sstring(std::string name,bool tpub)
{
  this->name=name;index=-1;
  type=3;pub=tpub;
    value="test";
  
}
SVec3::SVec3(std::string name,bool tpub)
{
  this->name=name;index=-1;
  type=3;pub=false;
  
}
SQuat::SQuat(std::string name,bool tpub)
{
  this->name=name;index=-1;
  type=3;pub=tpub;
}
// float 
void SFloat::update(std::string *table)
{
  lua_State *L=Script::L;

  setind(this);
  lua_pushnumber(L,value);
  
  lua_settable(L,-3);

}

void SFloat::read(std::string *table)
{
  lua_State *L=Script::L;
  getind(this);
  value=lua_tonumber(L,-1);
  lua_pop(L,1);
}


ScriptProp* SFloat::clone()
{
  SFloat* temps=new  SFloat(*this);
  return temps;
}

//boolean

void Sbool::update(std::string *table)
{
  lua_State *L=Script::L;
  setind(this);
  lua_pushboolean(L,value);
  
  lua_settable(L,-3);

}

void Sbool::read(std::string* table)
{
  lua_State *L=Script::L;
  getind(this);
  value=lua_toboolean(L,-1);
  lua_pop(L,1);
}


ScriptProp* Sbool::clone()
{
  ScriptProp* temps=new Sbool(*this);
return temps;
}


//string

void Sstring::update(std::string *table)
{
  lua_State *L=Script::L;
  setind(this);
  lua_pushstring(L,value.c_str());
  lua_settable(L,-3);
}

void Sstring::read(std::string *table)
{
  lua_State *L=Script::L;
  getind(this);
  value=lua_tostring(L,-1);
  lua_pop(L,1);
}


ScriptProp* Sstring::clone()
{
  ScriptProp* temps=new Sstring(*this);
return temps;
}
// vector 3

void SVec3::update(std::string *table)
{
  lua_State *L=Script::L;
  setind(this);
  setvec3(L,value);
  
  lua_settable(L,-3);


}

void SVec3::read(std::string *table)
{
  lua_State *L=Script::L;
  getind(this);
  int ind=lua_gettop(L);
  value=getvec3(L,ind);
  lua_pop(L,1);
}

ScriptProp* SVec3::clone()
{
  ScriptProp* temps=new  SVec3(*this);
  return temps;
}
// quaternion 
void SQuat::update(std::string *table)
{
  lua_State *L=Script::L;
  std::string tname= pub?"_"+name:name;

  lua_pushstring(L,tname.c_str());
  setquat(L,value);
  
  lua_settable(L,-3);

}

void SQuat::read(std::string *table)
{
  lua_State *L=Script::L;
  getind(this);
  int ind=lua_gettop(L);
  value=getquat(L,ind);
  lua_pop(L,1);
}

ScriptProp* SQuat::clone()
{
  ScriptProp* temps=new  SQuat(*this);
  return temps;
}


// Table

STable::STable(std::string tname)
{
  this->name=tname;
  type=7;pub=false;
}

STable::~STable()
{
    for(int i=0;i<tprops.size();i++)delete tprops[i];
    tprops.clear();
}

STable * STable::clone()
{
    STable* st=new STable(*this);
    st->tprops.clear();
    for(int i=0;i<tprops.size();i++)st->tprops.push_back( tprops[i]->clone());
    return st;
}

void STable::update(std::string *table)
{
  lua_State *L=Script::L;
  
  lua_pushstring(L,name.c_str());
  lua_newtable(L);
  {//
     for(int i=0;i<tprops.size();i++)
         tprops[i]->update(0);
  }//  
  lua_settable(L,-3);
}


void STable::read(std::string *table)
{
  for(int i=0;i<tprops.size();i++)delete tprops[i];
  tprops.clear();
    
  lua_State *L=Script::L;
    int tind=lua_gettop(L);
    lua_getfield(L,tind,name.c_str());
    // if table is set to null
    if(lua_isnil(L,-1)){lua_pop(L,1);return;}

    lua_pushvalue(L, -1);
    // stack now contains: -1 => table
    lua_pushnil(L);
    // stack now contains: -1 => nil; -2 => table
    while (lua_next(L, -2))
    {
        // stack now contains: -1 => value; -2 => key; -3 => table
        // copy the key so that lua_tostring does not modify the original
        lua_pushvalue(L, -2);
        // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
        int ikey=-1; 
        bool isindex=lua_isnumber(L,-1);
        if(isindex)ikey=lua_tonumber(L,-1);
        std:: string key = lua_tostring(L, -1);
        const char *value = lua_tostring(L, -2);
        
        ScriptProp* pr=0;
	///-- check if first character of the name of the variable is 'underscore'
	if(key!="type"){
      std::string tkey=key;

	  if     (lua_isnumber (L,-2)){
// 	    printf("It is a number!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
	    pr=new SFloat(tkey,false);
	    tprops.push_back(pr);
	    ((SFloat*)pr)->value=lua_tonumber(L,-2);
	  }
	  else if(lua_isboolean(L,-2)){
// 	    printf("It is a boolean!!!\n");
	    pr=new Sbool(tkey,false);
	    tprops.push_back(pr);
	    ((Sbool*)pr)->value=lua_toboolean(L,-2);
	  }
	  else if(lua_isstring (L,-2)){
// 	    printf("It is a string!!!\n" );
	    pr=new Sstring(tkey,false);
	    tprops.push_back(pr);
	    ((Sstring*)pr)->value=lua_tostring(L,-2);
	  }
      else if(lua_istable (L,-2)){
          int index=lua_gettop(L)-1;
          char type=0;
          lua_getfield(L,index,"type");
          if(lua_isnumber(L,-1)){
            type=lua_tonumber(L,-1);
            if(type==110){
                //get vector3
                pr=new SVec3(tkey,false);
                tprops.push_back(pr);
                ((SVec3*)pr)->value=getvec3(L,index);
            }
            else if(type==112){
                //get quat
                pr=new SQuat(tkey,false);
                tprops.push_back(pr);
                ((SQuat*)pr)->value=getquat(L,index);
            }
          }
          else{
              //get table
        // 	    printf("It is a table!!!\n" );
        // 	    pr=new STable(tkey);
        // 	    p_props.push_back(pr);// always private
        // 	    ((STable*)pr)->read(tkey);
              
          }
          lua_pop(L,1);
	  }
	  if(pr)pr->index=ikey;
	  
	}
        // pop value + copy of key, leaving original key
        lua_pop(L, 2);
        // stack now contains: -1 => key; -2 => table
    }
    // stack now contains: -1 => table (when lua_next returns 0 it pops the key
    // but does not push anything.)
    // Pop table
    lua_pop(L, 2);
  
    
}


