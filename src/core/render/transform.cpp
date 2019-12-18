#include"transform.h"
#include<stdio.h>
mat4 Transform::unit=mat4(1.0f);

Transform::Transform()
{
  position=vec3(0.0f);
  rotation=quat(0.0f,0.0f,0.0f,1.0f);
  scale=vec3(1.0f);
  center=vec3(0.0f);
  update();
}
 void Transform::update()
{
  model=mat4::Scale(scale);
  
  model.elements[12]-=center.x;
  model.elements[13]-=center.y;
  model.elements[14]-=center.z;
  
  model=mat4::Rotate(rotation)*model;
  
  model.elements[12]+=center.x;
  model.elements[13]+=center.y;
  model.elements[14]+=center.z;
  
  model.elements[12]+=position.x;
  model.elements[13]+=position.y;
  model.elements[14]+=position.z;
  
}
Transform Transform::inverse()
{
    Transform trans;
    trans.model=mat4::Inverse(model);

    trans.scale=vec3(1/scale.x,1/scale.y,1/scale.z);
    trans.rotation=rotation.inverse();
    trans.position=(trans.model.elements[12],trans.model.elements[13],trans.model.elements[14]);

    return trans;
}
void Transform::multiply(Transform& t)
{
    rotation=t.rotation*rotation;
    scale=t.scale*scale;
    model=t.model*model;
    position=vec3(model.elements[12],model.elements[13],model.elements[14]);
}

Transform Transform::operator*(Transform tr)
{
    Transform ttr=tr;ttr.multiply(*this);
    return ttr;
}


void Transform::setEuler(float x, float y, float z)
{
 rotation=quat::FromEulerAngles(vec3(x,y,z));
 
}

/// ////////////////////////////

Box::Box(vec3 l, vec3 u):ll(l),ul(u)
{
    odx=l+(u-l).x*vec3::Xaxis();
    ody=l+(u-l).y*vec3::Yaxis();
    odz=l+(u-l).z*vec3::Zaxis();
}
Box::Box(){}
void Box::tick(Transform& trans)
{
    L=trans.model*ll;
    U=trans.model*ul;
    dx=trans.model*odx;
    dy=trans.model*ody;
    dz=trans.model*odz;

}
vec3 RayPlane(vec3 ray,vec3 rp,vec3 n,vec3 pp){
    float r=(float)(pp-rp).Dot(n)/(float)(ray.Dot(n));
    return (rp+r*ray);
}
bool planeInside(vec3 n,vec3 L,vec3 U,vec3 p){
    float d1=(p-L).Dot(n),d2=(p-U).Dot(n);
    return  (d1>0 && d2<0); 
}
bool planeSide(vec3 n,vec3 L,vec3 p,float& d){
    d=(p-L).Dot(n);
    bool b=d>0;
    d*=d;
        return b;
}
bool Box::IntersectRay(vec3 ray, vec3 rpos,vec3& point)
{
   vec3 n[3];
   n[0]=(dx-L).Normalize();n[1]=(dy-L).Normalize();n[2]=(dz-L).Normalize();
   // correcting normal if scale is zero along any axis
        if(dx==L)n[0]=(dy-L).Cross((dz-L)).Normalize();
   else if(dy==L)n[1]=(dz-L).Cross(dx-L).Normalize();
   else if(dz==L)n[2]=(dx-L).Cross((dy-L)).Normalize();
   
   vec3 p1,p2;
   for(int i=0;i<3;i++){
     p1  = RayPlane(ray,rpos,-n[i],L);
     p2  = RayPlane(ray,rpos, n[i],U);
     bool b1=0,b2=0;
     
     for(int j=0;j<3;j++){
        if(j!=i && planeInside(n[j],L,U,p1)) {if(b1){point=p1;return true;} else{ b1=true;}}
        if(j!=i && planeInside(n[j],L,U,p2)) {if(b2){point=p2;return true;} else{ b2=true;}}
     }
   }
   return false;
}

bool Box::IntersectSphere(vec3 center, float radsq,float &dist)
{
   vec3 n[3];float dsq=radsq;float d=0;
   n[0]=(dx-L).Normalize();n[1]=(dy-L).Normalize();n[2]=(dz-L).Normalize();

   
   for(int i=0;i<3;i++){
        if(planeSide(-n[i],L,center,d)){dsq-=d;}
        else if(planeSide(n[i],U,center,d)){dsq-=d;}
   }
   dist=(radsq-dsq);
   return dsq>0;
}

bool Box::IntersectPlane(vec3 point, vec3 normal)
{
        Box &b=*this;vec3 ves[8];
        vec3 dx=b.dx-b.L,dy=b.dy-b.L,dz=b.dz-b.L;
        ves[1]=b.dx;ves[2]=b.dy;ves[3]=b.dz;
        ves[4]=b.L+dx+dy;ves[5]=b.L+dy+dz;
        ves[6]=b.L+dz+dx;ves[7]=b.U;ves[0]=b.L;
        // loop through aabb vertices
        for(short k=0;k<8;k++){
            if(normal.Dot(ves[k]-point)>0){
                return true;
            }
        }    
        return false;
}

void Box::getLightBounds(vec3 cp, vec3 cd, float* bounds)
{
     vec3 right=cd.Cross(vec3::Yaxis()).Normalize(),
          up=right.Cross(cd).Normalize();
     Box &b=*this;vec3 ves[8];float tbs[6];
        vec3 dx=b.dx-b.L,dy=b.dy-b.L,dz=b.dz-b.L;
        ves[1]=b.dx;ves[2]=b.dy;ves[3]=b.dz;
        ves[4]=b.L+dx+dy;ves[5]=b.L+dy+dz;
        ves[6]=b.L+dz+dx;ves[7]=b.U;ves[0]=b.L;
        // loop through aabb vertices
        for(short k=0;k<8;k++){
            vec3 tp=ves[k]-cp;
            /*
            tbs[0]=abs(tp.Dot(-right));
            tbs[1]=abs(tp.Dot(right));
            tbs[2]=abs(tp.Dot(-up));
            tbs[3]=abs(tp.Dot(up));
            tbs[4]=abs(tp.Dot(-cd));
            tbs[5]=abs(tp.Dot(cd));
            for(int l=0;l<6;l++){
                bounds[l]=Max(bounds[l],tbs[l]);
            }
            */
            bounds[4]=Max(bounds[4],abs(tp.Dot(-cd)));
        }    
}


