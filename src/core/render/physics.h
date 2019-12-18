#pragma once

#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include<vector>

class MyDebugDraw: public btIDebugDraw{
    int m_debugMode;
public:
    virtual void drawLine(const btVector3& from,const btVector3& to
                    ,const btVector3& color);
    virtual void drawContactPoint(const btVector3& PointOnB
                       ,const btVector3& normalOnB,btScalar distance
                       ,int lifeTime,const btVector3& color);
    virtual void reportErrorWarning(const char* warningString);
    virtual void draw3dText(const btVector3& location,const char* textString);
    virtual void setDebugMode(int debugMode);
    virtual int  getDebugMode() const;
    virtual bool  isVisible(const btVector3& aabbMin,const btVector3& aabbMax);

};

class MotionState: public btDefaultMotionState{
public:
    void getWorldTransform(btTransform & centerOfMassWorldTrans)const override;
    void setWorldTransform(const btTransform & centerOfMassWorldTrans) override;
     MotionState(const btTransform& startTrans,const btTransform& centerOfMassOffset);
     MotionState(const btTransform& startTrans);
};

class ContactTestCallback:public btCollisionWorld::ContactResultCallback{
public:
     ContactTestCallback(btCollisionObject *tgtBody,btCollisionObject* tavoid );
	 btCollisionObject *body,*avoid; //!< The body the sensor is monitoring
	 bool NeedsCollision(btBroadphaseProxy proxy);
     std::vector<btCollisionObject*> colls;
	//! Called with each contact for your own processing (e.g. test if contacts fall in within sensor parameters)
     btScalar addSingleResult(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0,int partId0,int index0,const btCollisionObjectWrapper* colObj1,int partId1,int index1);
};
