#pragma once
#include<fstream>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>

typedef unsigned short Ushort;
namespace fs = std::filesystem;

#define _SHADER_FOLDER "shaders/"
#define _IN_FOLDER "assets/"
#define _OUT "res/out.res"
#define _NOTFOLDER  fs::is_directory(dirname)

#define _MODEL (extension.compare(".FML")==0)
#define _SCENE (extension.compare(".scene")==0) && _NOTFOLDER
#define is_img(txt) extension.compare(txt)==0
#define _IMAGE (is_img(".jpg")||is_img(".png")||is_img(".tga")||is_img(".bmp"))
#define _AUDIO (extension.compare(".ogg")==0)
#define _SCRIPT (extension.compare(".lua")==0)
#define _FONT   (extension.compare(".otf")==0||extension.compare(".ttf")==0)

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
