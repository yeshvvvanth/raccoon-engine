#include"loader.h"

void saveScenes(Ushort& scn_num,std::ofstream& ofs,std::string f_path){
    std::string str,name;
    
    dirent* drnt;
    DIR *FOLDER=opendir(f_path.c_str());
//     printf("scenes path=%s\n",f_path.c_str());
    if(FOLDER){
        while ((drnt= readdir (FOLDER))){
            if(_SCENE){
                str.assign(drnt->d_name,strlen(drnt->d_name));
                name.assign(drnt->d_name,strlen(drnt->d_name));
                str=(f_path+str);
                    ///
                    unsigned char n_len=name.length();
            
                ofs.write((char*)&n_len,1);
                ofs.write((char*)name.c_str(),n_len);
                
                std::ifstream ifs(str,std::ios::binary);
                ifs.seekg(0,std::ios::end);
                unsigned int fsiz=ifs.tellg();
                
            // 	 printf("\tsize of %s file= %d \n",str.c_str(),fsiz);
                
                char* dataa=new char[fsiz];   //create buffer
                ifs.seekg(0,std::ios::beg);
                ifs.read((char*)dataa,fsiz);  //read
                

                ofs.write((char*)dataa,fsiz);  //write
                
                delete[] dataa; 
                
                ///
                scn_num++;
                printf("number of scenes = %d.....................................\n",scn_num);
            }
        }
    }
    closedir(FOLDER);
}
