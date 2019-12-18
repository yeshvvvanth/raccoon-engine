#include"physics.h"
#include"../data_types/shader.h"


void MotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const
{
    centerOfMassWorldTrans = m_graphicsWorldTrans * m_centerOfMassOffset.inverse(); 
}
void MotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans)
{
     m_graphicsWorldTrans = centerOfMassWorldTrans * m_centerOfMassOffset ;
}
MotionState::MotionState(const btTransform& startTrans)
:btDefaultMotionState(startTrans)
{
}
MotionState::MotionState(const btTransform& startTrans, const btTransform& centerOfMassOffset=btTransform::getIdentity())
:btDefaultMotionState(startTrans,centerOfMassOffset)
{
}


void MyDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
//     printf("fuku draw line \n");
    vec3 sp(from.x(),from.y(),from.z()),ep(to.x(),to.y(),to.z());
    vec3 colour(color.x(),color.y(),color.z());
//     printf("start x=%f  y=%f  z=%f \n\n",sp.x,sp.y,sp.z);
//     printf("end x=%f  y=%f  z=%f \n\n",ep.x,ep.y,ep.z);    
    Shader::drawLine(sp,ep,colour);
}

void MyDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    
    
}

void MyDebugDraw::reportErrorWarning(const char* warningString)
{
}

int MyDebugDraw::getDebugMode() const
{
    return m_debugMode;
}

void MyDebugDraw::setDebugMode(int debugMode)
{
    m_debugMode=(DebugDrawModes)debugMode;
}


void MyDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
}

bool MyDebugDraw::isVisible(btVector3 const& aabbmin, btVector3 const&aabbmax){
    // a function for frustum culling i guess
return true;
}
//////////// COLLISION TEST ///////////////

ContactTestCallback::ContactTestCallback(btCollisionObject *tgtBody,btCollisionObject *tavoid):ContactResultCallback()
{
    body=tgtBody;avoid=tavoid;
}
btScalar ContactTestCallback::addSingleResult(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0,int partId0,int index0,const btCollisionObjectWrapper* colObj1,int partId1,int index1)
{
        if(colObj0->getCollisionObject()==body ){
            if(colObj1->getCollisionObject()!=avoid)
                colls.push_back( (btCollisionObject*)colObj1->getCollisionObject());
        }
        else{
            if(colObj0->getCollisionObject()!=avoid)
                colls.push_back( (btCollisionObject*)colObj0->getCollisionObject());
        }
    return 0;
}
bool ContactTestCallback::NeedsCollision(btBroadphaseProxy proxy)
{
    
    return true;
}



