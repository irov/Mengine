#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include <NxPhysics.h>

#	include <NxCooking.h>

#	include <NxCapsuleController.h>

#	include <ControllerManager.h>

#include <windows.h>

#include <stdio.h>


    enum GameGroup
    {
        GROUP_NON_COLLIDABLE,
        GROUP_COLLIDABLE_NON_PUSHABLE,
        GROUP_COLLIDABLE_PUSHABLE,
    };

float getElapsedTime()
{
	static LARGE_INTEGER previousTime;
	static LARGE_INTEGER freq;
	static bool init = false;
	if(!init){
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&previousTime);
		init=true;
	}
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	unsigned long long elapsedTime = currentTime.QuadPart - previousTime.QuadPart;
	previousTime = currentTime;
	return (float)(elapsedTime)/(freq.QuadPart);
}

class MyReport : public NxUserControllerHitReport
{
	public:

	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
	{
		printf("hit\n");
		return NX_ACTION_NONE;
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}
}
gMyReport;

static NxController * Controller;

NxU32 moveCharacter(NxScene& scene, const NxVec3& dispVector, NxF32 elapsedTime, NxU32 collisionGroups, NxF32 heightDelta)
{
	NxF32 sharpness = 1.0f;

	NxU32 collisionFlags;
	NxVec3 d = dispVector*elapsedTime;

	if(heightDelta!=0.0f)
	{
			d.y+=heightDelta;
	}
	
	Controller->move(d, collisionGroups, 0.000001f, collisionFlags, sharpness);
	
	NxExtendedVec3 v = Controller->getFilteredPosition();
//	printf("Contrloller = %f %f %f\n", v.x, v.y, v.z);
	return collisionFlags;
}

const NxExtendedVec3& getCharacterPos()
{
	return Controller->getFilteredPosition();
}

NxActor* getCharacterActor()
{
	return Controller->getActor();
}

void reportSceneChanged() 
{
	Controller->reportSceneChanged();
}

bool resetCharacterPos(const NxVec3& pos)
{
	return Controller->setPosition(NxExtendedVec3(pos.x, pos.y, pos.z));
}

/*static NxRay gWorldRay;

static NxF32 cameraHit(NxScene		 * gScene)
{
	NxShape* characterShape = getCharacterActor()->getShapes()[0];

	NxExtendedVec3 cpos = getCharacterPos();

	NxRay worldRay;
	worldRay.orig.x	= cpos.x;
	worldRay.orig.y	= cpos.y;
	worldRay.orig.z	= cpos.z;
	worldRay.dir	= -gDir;
	NxRaycastHit hit;
	characterShape->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	NxShape* shape = gScene->raycastClosestShape(worldRay, NX_ALL_SHAPES, hit);
	characterShape->setFlag(NX_SF_DISABLE_RAYCASTING, false);

	gWorldRay = worldRay;

	return shape ? hit.distance : NX_MAX_F32;
}*/

class NovodexPhysicSystem: public PhysicSystemInterface
{
public:
	NovodexPhysicSystem();
	~NovodexPhysicSystem();
public:
	void init( float gx, float gy, float gz ) override;
	void update( float _timestep ) override;
public:
	void setGravity( float _x, float _y, float _z ) override;
	void setRestitution( float _value ) override;
	void setStaticFriction( float _value ) override;
	void setDynamicFriction( float _value ) override;
	float rayCast(const char * _name, float * pos, float * dir);
public:

	ControllerManager gCM;

	#define SKINWIDTH	0.9f

	void moveController( float * _dispvec )
	{
		//NxF32 elapsedTime = getElapsedTime();
		NxF32 elapsedTime = 1;
		moveCharacter(*m_scene, *(NxVec3*)_dispvec,elapsedTime,6,0.0f);
	}

	float * getControllerPos()
	{
		static	NxExtendedVec3 v;
		static float p[3];
		v = Controller->getFilteredPosition();
		p[0] = v.x;
		p[1] = v.y;
		p[2] = v.z;
		return  p;
	}

	void createController( float * _startPos, float _initialRadius, float _initialHeight )
	{
		NxCapsuleControllerDesc desc;

		desc.interactionFlag	= NXIF_INTERACTION_INCLUDE;

		desc.position.x			= _startPos[0];
		desc.position.y			= _startPos[1];
		desc.position.z			= _startPos[2];
		desc.radius				= _initialRadius;
		desc.height				= _initialHeight;
		desc.upDirection		= NX_Y;
		desc.slopeLimit			= 0.0f;
		desc.slopeLimit			= cosf(NxMath::degToRad(45.0f));
		desc.skinWidth			= SKINWIDTH;
		desc.stepOffset			= _initialRadius;
//		desc.stepOffset			= 0.0f;
//		desc.stepOffset			= 10.0f;
		desc.callback			= &gMyReport;

		Controller = (NxCapsuleController*)gCM.createController(m_scene, desc);
	};



	GeometryInterface * cookConvex( const float * _verts, int _vertexSize ) override;
	GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) override;
	GeometryInterface * cookConvex( const char * _filename ) override;
	GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) override;
	GeometryInterface * cookConcave( const char * _filename ) override;
	GeometryInterface * cookBox( float _width, float _height, float _depth ) override;
	void removeGeometry( GeometryInterface * _geom ) override;
public:
	RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry) override;
	void	removeRigidBody( RigidBodyInterface * _rigidBody ) override;
public:
	void	createJoint(RigidBodyInterface * body0, RigidBodyInterface * body1, float x, float y, float z) override;
private:
	NxPhysicsSDK * m_physicsSDK;
	NxScene		 * m_scene;
	NxSceneQuery*    m_SceneQueryObject;
};