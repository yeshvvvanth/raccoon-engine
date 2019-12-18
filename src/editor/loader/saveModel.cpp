#include"loader.h"
///////////////////////////////
void saveModel(std::ofstream& ofs, std::string& str,std::string& t_name)
{
    unsigned short num_mesh=0;
    const char* name;unsigned char n_len=0;
    
    std::ifstream ifs(str,std::ios::binary);
    ifs.seekg(0,std::ios::end);
    unsigned int fsiz=ifs.tellg();fsiz-=2;
    
    printf("\tsize of %s file= %d \n",str.c_str(),fsiz);
    char* dataa=new char[fsiz];                //create buffer
    
    ifs.seekg(0,std::ios::beg);
    ifs.read((char*)&num_mesh,2);   //read number of meshes 
    ifs.read((char*)dataa,fsiz);  //read
    
    
    name=t_name.c_str();
    unsigned char dt=0,t_len=t_name.length();
    
    for(unsigned char u=t_len-1;u>0;u--){
       if(t_name[u]=='.' && dt==0){dt=u;break;}
    };
    n_len=(dt)+1; // one extra for the seperator
//     printf("** %s %d **\n",name,n_len);
    
    //name=new char[n_len];
    ofs.write((char*)&num_mesh,2);
    ofs.write((char*)&n_len,1);
    ofs.write((char*)name,n_len-1);
    char seperator='#';
    ofs.write((char*)&seperator,1);
    
    //delete[] name;
    
    ofs.write((char*)dataa,fsiz);  //write
    
    delete[] dataa;                  // add to number of assets
}
