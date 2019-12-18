#include"loader.h"
void saveFile(FILE*,std::ofstream&);
/////////////////////////////////////////////////////////////////
void saveShaders(Ushort &sha_num,std::ofstream& ofs,std::string f_path){
    dirent* drnt;
    DIR *FOLDER=opendir(f_path.c_str());
    if(FOLDER){
        while ((drnt= readdir (FOLDER))){
        if(drnt->d_type==DT_DIR && (drnt->d_name[0]!='.')){
            sha_num++;
            DIR *SDIR=opendir((f_path+drnt->d_name).c_str());
            unsigned char n_len=strlen(drnt->d_name);
            //printf("%d %s : \n",n_len,drnt->d_name);
            ofs.write((char*)&n_len,1);
            ofs.write((char*)&drnt->d_name,n_len);
            
            FILE* fp=fopen((f_path+drnt->d_name+"/"+drnt->d_name+".vert").c_str(),"rt");
            saveFile(fp,ofs);
            fp=fopen((f_path+drnt->d_name+"/"+drnt->d_name+".frag").c_str(),"rt");
            saveFile(fp,ofs);
            
            closedir(SDIR);
        }
        }
    }
    closedir(FOLDER);
    printf("____________________________\n");
}
void saveFile(FILE* filep,std::ofstream &ofs)
{
 if(filep!=NULL){
   char * buff;
   fseek(filep,0,SEEK_END);
   unsigned short size=ftell(filep);
   //printf("-----%d-----\n",size);
   size++;
   buff=new char[size];buff[size-1]='\0';
   fseek(filep,0,SEEK_SET);
   fread(buff,1,size,filep);
   //printf("%s\n",buff);
   
   ofs.write((char*)&size,2);
   ofs.write((char*)buff,size);
   delete[] buff;
 }
}


