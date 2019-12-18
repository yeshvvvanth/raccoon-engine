#include<stdio.h>
#include"../maths.h"
#include<glm/glm.hpp>
    #include<glm/gtc/matrix_transform.hpp>
    #include<glm/gtc/type_ptr.hpp>

int main(){
 
    float el[16],pl[16];
    for(Uchar u=0;u<16;u++){
        el[u]=u*1.0f;
        pl[u]=23.0f-u*1.0f;
    }
    mat4 mat(6.0f);
 //quat q(4.0f,5.0f,6.0f,7.0f);
 //glm::quat gq=glm::quat(4.0f,5.0f,6.0f,7.0f);
 
 //q= q+      quat(1.0f,2.0f,3.0f,4.0f)*5.0f+     quat(10.0f,21.0f,36.0f,88.0f)*9.0f;
 //gq=gq+glm::quat(1.0f,2.0f,3.0f,4.0f)*5.0f+glm::quat(10.0f,21.0f,36.0f,88.0f)*9.0f;
 mat4 m1=mat4(pl)*mat4(el);
 //mat4 m1=mat*mat4::Invert(mat);
 for(Uchar u=0;u<4;u++){
     for(Uchar v=0;v<4;v++){
//          printf("  %.1f",m1.elements[v+u*4]);
    }printf("\n");
}
 //printf("glm: %.1f %.1f %.1f %.1f \n",gq.w,gq.x,gq.y,gq.z);
 //printf("myquat: %.1f %.1f %.1f %.1f \n",q.w,q.x,q.y,q.z);
 return 0;
}
