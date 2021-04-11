#include"loader.h"
#include<vector>
///////////////
struct Cmap{
    unsigned char b;
    std::string name;
    unsigned char *buffers[6];
    int   sizes[6];
    static std::vector<Cmap*> cmaps;
};
std::vector<Cmap*> Cmap::cmaps;
///////////////////////////////////////////////////////////////////
void saveImage(std::ofstream& ofs,std::string& file_path,std::string& name,Ushort &ass_num)
{
    unsigned char type=2;     // ******10
    
    if(name.compare(0,3,"cm_")==0){type+=4; for(int j=0;j<6;j++)printf("cubemap %s \n",name.c_str());}
    
    if(type==2){
        saveRaw(ofs,file_path,name,2);
        ass_num++;
    }
    else{
        Cmap* cm=0;
        for(int i=0;i<Cmap::cmaps.size();i++)
            {
//                 printf("@@@@@@@@@ %s || %s ^^^^^^\n\n",name.c_str()+5,Cmap::cmaps[i]->name.c_str());
                if(strcmp(name.c_str()+5,Cmap::cmaps[i]->name.c_str() )==0 ){cm=Cmap::cmaps[i];break;}
            }
        if(!cm)
        {
            cm=new Cmap;
            cm->name.assign(&name[5],name.length()-5);
//             printf("^^^^^^^^^^^^^^^^namee = %s ^^^^^^\n\n",cm->name.c_str());
            Cmap::cmaps.push_back(cm);
            cm->b=0;
            for(int j=0;j<6;j++)
                cm->buffers[j]=0;
        }
        std::ifstream ifs(file_path.c_str(),std::ios::binary);
        int len=0;
        unsigned char *mem=0;
        ifs.seekg(0,std::ios::end);
        len=ifs.tellg();
        ifs.seekg(0,std::ios::beg);
        mem=new unsigned char[len];
        ifs.read((char*)mem,len);
        
             if(name[3]=='r'){cm->buffers[0]=mem;cm->sizes[0]=len;cm->b+=1;}
        else if(name[3]=='l'){cm->buffers[1]=mem;cm->sizes[1]=len;cm->b+=2;}
        else if(name[3]=='u'){cm->buffers[2]=mem;cm->sizes[2]=len;cm->b+=4;}
        else if(name[3]=='d'){cm->buffers[3]=mem;cm->sizes[3]=len;cm->b+=8;}
        else if(name[3]=='b'){cm->buffers[4]=mem;cm->sizes[4]=len;cm->b+=16;}
        else if(name[3]=='f'){cm->buffers[5]=mem;cm->sizes[5]=len;cm->b+=32;}
        
    }
    
}
void saveCubemaps(Ushort &ass_num,std::ofstream& ofs){
    for(int i=0;i<Cmap::cmaps.size();i++){
        unsigned char n_len= Cmap::cmaps[i]->name.length(),type=2+4;
        unsigned short num_img=1;
        for(int j=0;j<6;j++)printf("__++++++++++_______+++++++++_______b= %d \n\n",Cmap::cmaps[i]->b);
        if(Cmap::cmaps[i]->b==63){//63
            ass_num++;
            ofs.write((char*)&num_img,2);
            ofs.write((char*)&n_len,1);
            ofs.write((char*)Cmap::cmaps[i]->name.c_str(),n_len);
    
            ofs.write((char*)&type,1);
            for(int j=0;j<6;j++)
            {
                ofs.write((char*)&Cmap::cmaps[i]->sizes[j],4);          //buffer size
                ofs.write((char*)Cmap::cmaps[i]->buffers[j],Cmap::cmaps[i]->sizes[j]); //buffer itself
            }
        }
        for(int j=0;j<6;j++)
            {if(Cmap::cmaps[i]->buffers[j])delete [] Cmap::cmaps[i]->buffers[j];}
        delete Cmap::cmaps[i];
    }
    Cmap::cmaps.clear();
    
}
