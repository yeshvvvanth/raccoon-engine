#include"data.h"
#include"../UI/textButton.h"
void loadPrefs(std::ifstream& ifs);
bool firstload=true;
Data::Data(std::string d_path)
{

  d_path+="out.res";
  path=d_path;
  printf("path = %s \n",d_path.c_str());
  std::ifstream ifs(d_path,std::ios::binary);
  if(!ifs.good()){
      perror("Resource File Wasn't Found !\n");
      //throw "Resource File Wasn't Found !\n";
      exit(0);
  }
  unsigned short num_assets=0,num_shaders=0,num_scenes=0,num_editor_assets=0;
  ifs.read((char*)&num_shaders,sizeof(num_shaders));
  ifs.read((char*)&num_editor_assets,sizeof(num_editor_assets));
  ifs.read((char*)&num_assets,sizeof(num_assets));
  ifs.read((char*)&num_scenes,sizeof(num_scenes));
  if(num_shaders==0){perror("No Shader Source!\n");exit(0);}
  loadShaders(ifs,num_shaders);
  loadAssets(ifs,num_editor_assets,true);
  fpos=ifs.tellg();
  loadAssets(ifs,num_assets,false);
  Font::init();
  loadScenes(ifs,num_scenes);
  
  loadPrefs(ifs);
}
void Data::reload()
{
    std::vector<std::string> asnames;
    asnames.reserve(Scene::ascenes.size());
    //temp saving the ascne names
    for(int i=0;i<Scene::ascenes.size();i++){
        asnames.push_back(Scene::ascenes[i]->name);
    }
    
   for(unsigned short u=0;u<scenes.size();u++)delete scenes[u];
   scenes.clear();Scene::scenes.clear();Scene::ascenes.clear();
   for(unsigned short u=0;u<meshes.size();u++)delete meshes[u];
   meshes.clear();RawMesh::meshes.clear();
    for(unsigned short u=0;u<textures.size();u++)delete textures[u];
    textures.clear();Texture::textures.clear();
    for(unsigned short u=0;u<Audio::audios.size();u++)delete Audio::audios[u];
    for(unsigned short u=0;u<Font::fonts.size();u++)delete Font::fonts[u];
   Font::fonts.clear();
   Audio::audios.clear();
   Script::end();
    /// ////////////////
   printf("path = %s \n",path.c_str());
  std::ifstream ifs(path,std::ios::binary);
  if(!ifs.good()){
      perror("Resource File Wasn't Found !\n");
      //throw "Resource File Wasn't Found !\n";
      exit(0);
  }
  unsigned short num_ass=0,scn_num=0;
    ifs.seekg(4,std::ios::beg);
    ifs.read((char*)&num_ass,sizeof(num_ass));
    ifs.read((char*)&scn_num,sizeof(scn_num));
    
    Script::begin();
    ifs.seekg(fpos,std::ios::beg);
    loadAssets(ifs,num_ass,false);
    loadScenes(ifs,scn_num);

    loadPrefs(ifs);
    
    if(!Scene::ascenes.size()&&Scene::scenes.size())Scene::scenes[0]->setActive();
    else if(Scene::scenes.empty()){
      Scene *scn=new Scene();
      scenes.push_back(scn);Scene::scenes.push_back(scn);
      Scene::ascenes.push_back(scn);
      scn->setActive();
    }
}

Data::~Data()
{
   for(unsigned short u=0;u<scenes.size();u++)delete scenes[u];
   scenes.clear();Scene::scenes.clear();Scene::ascenes.clear();
   for(unsigned short u=0;u<RawMesh::emeshes.size();u++)delete RawMesh::emeshes[u];
   RawMesh::emeshes.clear();
   for(unsigned short u=0;u<textures.size();u++)delete textures[u];
   textures.clear();Texture::textures.clear();
   for(unsigned short u=0;u<CubeMap::cubemaps.size();u++)delete CubeMap::cubemaps[u];
   CubeMap::cubemaps.clear();
   for(unsigned short u=0;u<shaders.size();u++)delete shaders[u];
   shaders.clear();Shader::shaders.clear();
   for(unsigned short u=0;u<Audio::audios.size();u++)delete Audio::audios[u];
   
   for(unsigned short u=0;u<meshes.size();u++)delete meshes[u];
   meshes.clear();RawMesh::meshes.clear();
   for(unsigned short u=0;u<Texture::etextures.size();u++)delete Texture::etextures[u];
   Texture::etextures.clear();
   for(unsigned short u=0;u<CubeMap::ecubemaps.size();u++)delete CubeMap::ecubemaps[u];
   CubeMap::ecubemaps.clear();
   
   Audio::audios.clear();
   Script::end();
}

void Data::loadScenes(std::ifstream& ifs, short unsigned int& scn_num)
{
  for(int i=0;i<scn_num;i++){
     Scene* tscene=new Scene(this);
     tscene->read(ifs);
     scenes.push_back(tscene);
//      printf("&&&&&&&&&&&&&&& Scene name yooo -%s- \n",tscene->name.c_str());
     if(tscene->name=="_common_")Scene::common=tscene;
     else if(tscene->name=="_prefabs_")Scene::prefabs=tscene;
  }
  Scene::scenes=scenes;
}


void Data::loadAssets(std::ifstream& ifs, short unsigned int& num_ass,bool eda)
{
   
  unsigned char type=0,n_len=0;char *a_name;
  unsigned short num_mesh=0;
  Script::success=true;
  for(int i=0;i<num_ass;i++){
    ifs.read((char*)&num_mesh,2);
    
    ifs.read((char*)&n_len,1);
    a_name=new char[n_len+1];a_name[n_len]='\0';
    ifs.read((char*)a_name,n_len);
    //printf("********a_name = %s \n",a_name);
//          printf("__________NEW MESH____________\n");
    
    for(unsigned short u=0;u<num_mesh;u++){
       ifs.read((char*)&type,1);
       if(type&T_MESH){
            Shader* shader;
            if(type& MESH_ANIM){shader=Shader::def;}
            else {shader=Shader::def;}
            
//             shader=Shader::pervert;
            
            if(eda)shader=Shader::simple;
            RawMesh* mesh=new RawMesh(type,ifs,shader);                        // m_name= filename
            mesh->name=std::string(a_name)+ mesh->name;        //m_name = filename + name of mesh 
            
//             printf("********a_name =%s  %s \n",a_name,mesh->name.c_str());

            if(eda){
                if(firstload)
                RawMesh::emeshes.push_back(mesh);
                else
                delete mesh;
            }
            else 
                meshes.push_back(mesh);
       }
       else if(type&T_TEXTURE){
           if(type&4)
           {
               //read cubemap
               CubeMap* cm=new CubeMap(a_name,ifs);
                if(eda){
                    if(firstload)
                    CubeMap::ecubemaps.push_back(cm);
                    else
                    delete cm;
                }
                else 
                 CubeMap::cubemaps.push_back(cm);
           }
           else{
            Texture* texture =new Texture(type,ifs);
            texture->name=std::string(a_name);
            
            if(eda){
                if(firstload)
                Texture::etextures.push_back(texture);
                else
                delete texture;
            }
            else 
                textures.push_back(texture);

  
//             printf("image width=%d height=%d\n",textures[0]->width,textures[0]->height);
           }
       }
       else if(type&T_AUDIO){Audio* audio=new Audio(type,ifs);audio->name.assign(a_name);}
       else if(type&T_FONT){
           Font* fnt=new Font(a_name,ifs,38,eda);
            if(eda){
                if(firstload)
                Font::efonts.push_back(fnt);
                else
                delete fnt;
            }
            else 
                Font::fonts.push_back(fnt);

    }
       else if(type&T_SCRIPT){Script::read(ifs,eda,a_name);}
       //printf("%d type = %d ; fpos= %d\n",i,type,ifs.tellg());
    }
    delete[] a_name;
  }
   RawMesh::meshes=meshes;
   Texture::textures=textures;
   if(eda){Script::begin();}
   else{Script::registerScripts();}
   firstload=false;
}


void Data::loadShaders(std::ifstream &ifs,unsigned short &sha_num)
{
  
  for(unsigned short i=0;i<sha_num;i++){
    GLchar *vert,*frag;
    unsigned short vert_size,frag_size;unsigned char typ=0;
    //
    unsigned char n_len;char* sname;
    ifs.read((char*)&n_len,1);
    sname=new char[n_len+1];sname[n_len]='\0';
    ifs.read((char*)sname,n_len);
    ///
    ifs.read((char*)&vert_size,2);vert=new GLchar[vert_size];
    ifs.read((char*)vert,vert_size);
    ifs.read((char*)&frag_size,2);frag=new GLchar[frag_size];
    ifs.read((char*)frag,frag_size);
    shaders.push_back(new Shader(sname,vert,frag));
    
    //delete[] vert;delete[] frag;
  }
     Shader::shaders=shaders;
     Shader::def=Shader::findShader("default");
     Shader::render2D=Shader::findShader("render2D");
     Shader::simple=Shader::findShader("simple");
     Shader::fontShader=Shader::findShader("render2D");
     Shader::pervert=Shader::findShader("pervert");
     Shader::terrain=Shader::findShader("terrain");
     
     Sprite::shader=Shader::render2D;
     Font::shader=Shader::fontShader;
     Camera::data=this;
}


RawMesh* Data::findRawMesh(const char* mesh_name)
{
  for(int i=0;i<meshes.size();i++){
    if(strcmp(mesh_name,meshes[i]->name.c_str())==0){return meshes[i];}
  }
  return 0;
}
Texture* Data::findTexture(const char* tex_name)
{
  for(int i=0;i<textures.size();i++){
    if(strcmp(tex_name,textures[i]->name.c_str())==0){return textures[i];}
  }
  return 0;
}

void loadPrefs(std::ifstream& ifs)
{
    bool b=0;
    ifs.read((char*)&b,1);
    Scene *s=0;
    if(b){
        ifs.read((char*)Camera::default_camera->pos.elements,4*3);
        ifs.read((char*)Camera::default_camera->rot.elements,4*4);
        
        short sn=0,nlen=0;char *name=0;
        ifs.read((char*)&sn,2);
        for(int a=0;a<sn;a++){
            ifs.read((char*)&nlen,1);
            name=new char[nlen+1];name[nlen]='\0';
            ifs.read((char*)name,nlen);
            s=Scene::findScene(name);
            if(s){Scene::ascenes.push_back(s);}
            delete []name;
        }
    }
    if(Scene::ascenes.size())Scene::ascenes[0]->setActive();
}


