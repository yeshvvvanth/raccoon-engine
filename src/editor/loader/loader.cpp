#include"loader.h"

  DIR* dir;       // directory pointer aka folder pointer
  dirent* drnt;  // directory entry aka file structure pointer
  void savePrefs(std::ofstream& ofs,std::string folder);
void saveAssets(Ushort &ass_num,std::ofstream& ofs,std::string dir);
void loadAss(std::string Gdir,std::string RPATH){
  Ushort sha_num=0;
  Ushort eass_num=0;
  Ushort ass_num=0;
  Ushort scn_num=0;

  Gdir+="../";
  std::string ASS_Folder,SHA_Folder,DEF_SHA_Folder,SCN_FOLDER,RES_File,DEF_ASS_Folder;
  ASS_Folder=Gdir+"/assets/";
  SHA_Folder=Gdir+"/shaders/";
  DEF_SHA_Folder=RPATH+"/shaders/";
  DEF_ASS_Folder=RPATH+"/assets/";

  SCN_FOLDER=Gdir+"/scenes/";
  RES_File=Gdir+"/out/out.res";
  
  
std::ofstream ofs(RES_File,std::ios::binary);
ofs.write((char*)&sha_num,sizeof(sha_num));
ofs.write((char*)&eass_num,sizeof(eass_num));
ofs.write((char*)&ass_num,sizeof(ass_num));
ofs.write((char*)&scn_num,sizeof(scn_num));

      saveShaders(sha_num,ofs,SHA_Folder);
      saveShaders(sha_num,ofs,DEF_SHA_Folder);
      
      saveAssets(eass_num,ofs,DEF_ASS_Folder);
      saveAssets(ass_num,ofs,ASS_Folder);
      
      saveScenes(scn_num,ofs,SCN_FOLDER);
      savePrefs(ofs,SCN_FOLDER);
      
  
  printf("number of shaders=%d\n",sha_num);
  printf("number of assets=%d\n",ass_num);
  ofs.seekp(0,std::ios::beg);
  ofs.write((char*)&sha_num,sizeof(sha_num));
  ofs.write((char*)&eass_num,sizeof(eass_num));
  ofs.write((char*)&ass_num,sizeof(ass_num));
  ofs.write((char*)&scn_num,sizeof(scn_num));
  ofs.close();

}
void saveAssets(Ushort& ass_num, std::ofstream& ofs, std::string dirname)
{
    std::string path,name;
    
    dir=opendir(dirname.c_str());
    if (dir != NULL)
    {
        while ((drnt= readdir (dir))){
            if(_NOTFOLDER){
                path.assign(drnt->d_name,strlen(drnt->d_name));
                name.assign(drnt->d_name,strlen(drnt->d_name));
                path=(dirname+path);
                //printf("%s\n",path.c_path());
                if(_MODEL){saveModel(ofs,path,name);ass_num++;}
                else if(_IMAGE) {saveImage(ofs,path,name,ass_num);}
                else if(_AUDIO){saveRaw(ofs,path,name,4);ass_num++;}
                else if(_SCRIPT){saveRaw(ofs,path,name,8);ass_num++;}
                else if(_FONT){saveRaw(ofs,path,name,16);ass_num++;}
            }      
        }
    }
    else perror ("Couldn't open the directory");
      
    closedir (dir);
    saveCubemaps(ass_num,ofs);
}

void saveRaw(std::ofstream& ofs,std::string& path,std::string& name,unsigned char type){
    
    std::ifstream ifs(path.c_str(),std::ios::binary);
    int len=0;
    unsigned char *mem=0;
    ifs.seekg(0,std::ios::end);
    len=ifs.tellg();
    ifs.seekg(0,std::ios::beg);
    mem=new unsigned char[len];
    ifs.read((char*)mem,len);
    
    unsigned char nlen=name.length();
        
    unsigned short tnum=1;
    ofs.write((char*)&tnum,2);
    
    ofs.write((char*)&nlen,1);           // name length 
    ofs.write((char*)name.c_str(),nlen); // name 
    
    ofs.write((char*)&type,1);         //type         
    
    ofs.write((char*)&len,4);          //buffer size
    ofs.write((char*)mem,len); //buffer itself
    delete []mem;
}
void savePrefs(std::ofstream& ofs, std::string sfolder)
{
    std::ifstream ifs(sfolder+".project");
    bool b=(ifs.good());
    
    ofs.write((char*)&b,1);
    if(b){
        ifs.seekg(0,std::ios::end);
        unsigned int fsize=ifs.tellg();
        char* data=new char[fsize];
        ifs.seekg(0,std::ios::beg);
        ifs.read((char*)data,fsize);
        
//         ofs.write((char*)&fsize,4);
        ofs.write((char*)data,fsize);
        delete []data;
    }
}

void makedirs(std::string Gdir)
{
     mkdir((Gdir+"/res").c_str(),S_IRWXU);  
     mkdir((Gdir+"/assets").c_str(),S_IRWXU);  
     mkdir((Gdir+"/scenes").c_str(),S_IRWXU);  
     mkdir((Gdir+"/out").c_str(),S_IRWXU);  
     //mkdir((Gdir+"/shaders").c_str(),S_IRWXU);  
}

