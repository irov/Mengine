#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include <NxPhysics.h>

#	include <NxCooking.h>

#	include <NxCapsuleController.h>

#	include <ControllerManager.h>

#	define SKINWIDTH	0.1f

enum GameGroup
{
    GROUP_NON_COLLIDABLE,
    GROUP_COLLIDABLE_NON_PUSHABLE,
    GROUP_COLLIDABLE_PUSHABLE,
};

#	define COLLIDABLE_MASK	(1<<GROUP_COLLIDABLE_NON_PUSHABLE) | (1<<GROUP_COLLIDABLE_PUSHABLE)

class NovodexPhysicSystem: public Menge::PhysicSystemInterface
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
	float * getGravity() const override;
public:

	Menge::ControllerInterface * createCapsuleController( float * _startPos, float _initialRadius, float _initialHeight ) override;
	void releaseCapsuleController( Menge::ControllerInterface * _capsule ) override;

	Menge::GeometryInterface * cookConvex( const float * _verts, int _vertexSize ) override;
	Menge::GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) override;
	Menge::GeometryInterface * cookConvex( const char * _filename ) override;
	Menge::GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) override;
	Menge::GeometryInterface * cookConcave( const char * _filename ) override;
	Menge::GeometryInterface * cookBox( float _width, float _height, float _depth ) override;
	void removeGeometry( Menge::GeometryInterface * _geom ) override;
public:
	Menge::RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const Menge::GeometryInterface * _geometry) override;
	void	removeRigidBody( Menge::RigidBodyInterface * _rigidBody ) override;
public:
	void	createJoint( Menge::RigidBodyInterface * body0, Menge::RigidBodyInterface * body1, float x, float y, float z ) override;
private:
	NxPhysicsSDK * m_physicsSDK;
	NxScene		 * m_scene;
	ControllerManager m_controllerManager;
};