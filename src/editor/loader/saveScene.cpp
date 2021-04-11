#include"loader.h"
#include <iostream>

using namespace std;
void saveScenes(Ushort& scn_num,std::ofstream& ofs,std::string dir_path){
    std::string file_path,name;
    std::string dirname = dir_path;

     if(fs::exists(dirname) && fs::is_directory(dirname)){

        for (const auto& entry : fs::recursive_directory_iterator(dirname))
        {
            auto filename = entry.path().filename();
            std::string extension =  filename.extension();

            if (fs::is_directory(entry.status()))
            {
                cout << "[+] DIR:: " << filename << "\n";
                // DisplayDirTree(entry, 1);
                cout << "\n";
            }
            else if (fs::is_regular_file(entry.status())){
                
                // cout << "FILENAME: " << drnt->d_name;

                if(_SCENE){
                    
                    file_path.assign(filename);
                    name.assign(filename);
                    file_path = entry.path();
                        ///
                        unsigned char n_len=name.length();
                
                    ofs.write((char*)&n_len,1);
                    ofs.write((char*)name.c_str(),n_len);
                    
                    std::ifstream ifs(file_path,std::ios::binary);
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
        
        

                cout << "FILE: " << filename << "\n";
                cout << "FILE: " << filename.extension() << "\n";
                cout << "FILE: " << filename.filename() << "\n";
            }
            else
                cout << " [?]" << filename << "\n";
        }
    }
    
    
}
