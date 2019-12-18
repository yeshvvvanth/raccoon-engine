#pragma once
#include<fstream>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef unsigned short Ushort;

#define _SHADER_FOLDER "shaders/"
#define _IN_FOLDER "assets/"
#define _OUT "res/out.res"
#define _NOTFOLDER (drnt->d_type!=DT_DIR &&drnt->d_name[0]!= '.'&& drnt->d_name[strlen(drnt->d_name)-1]!='~')

#define _MODEL (strcmp(drnt->d_name+strlen(drnt->d_name)-4,".FML")==0)
#define _SCENE (strcmp(drnt->d_name+strlen(drnt->d_name)-6,".scene")==0) && _NOTFOLDER
#define iimg(txt) strcmp(drnt->d_name+strlen(drnt->d_name)-4,txt)==0
#define _IMAGE (iimg(".jpg")||iimg(".png")||iimg(".tga")||iimg(".bmp"))
#define _AUDIO (strcmp(drnt->d_name+strlen(drnt->d_name)-4,".ogg")==0)
#define _SCRIPT (strcmp(drnt->d_name+strlen(drnt->d_name)-4,".lua")==0)
#define _FONT   (strcmp(drnt->d_name+strlen(drnt->d_name)-4,".otf")==0||strcmp(drnt->d_name+strlen(drnt->d_name)-4,".ttf")==0)

void loadAss(std::string,std::string);
void saveImage(std::ofstream& ofs,std::string& str,std::string& name,Ushort &ass_num);
void saveModel(std::ofstream& ofs,std::string& str,std::string& name);

void saveShaders(Ushort &sha_num,std::ofstream& ofs,std::string);
void saveScenes(Ushort &scn_num,std::ofstream& ofs,std::string f_path);
/*
void saveAudio(std::ofstream& ofs,std::string& str,std::string& name);
void saveScript(std::ofstream& ofs,std::string& str,std::string& name);
void saveFont(std::ofstream& ofs,std::string& str,std::string& name);
*/
void saveRaw(std::ofstream& ofs,std::string& path,std::string& name,unsigned char type);
void saveCubemaps(Ushort &ass_num,std::ofstream& ofs);
void makedirs(std::string);
