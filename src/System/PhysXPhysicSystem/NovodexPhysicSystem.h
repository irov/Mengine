#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include <NxPhysics.h>

#	include <NxCooking.h>

class NovodexPhysicSystem: public PhysicSystemInterface
{
public:
	NovodexPhysicSystem();
	~NovodexPhysicSystem();

	void	init( float gx, float gy, float gz ) override;
	void	update( float timestep ) override;

	GeometryInterface * cookConvex( const float * _verts, int _vertexSize ) override;
	GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) override;
	GeometryInterface * cookConvex( const char * _filename ) override;
	GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) override;
	GeometryInterface * cookConcave( const char * _filename ) override;
	GeometryInterface * cookBox( float _width, float _height, float _depth ) override;

	RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry) override;
	void	removeRigidBody( RigidBodyInterface * _rigidBody ) override;

	void	createJoint(RigidBodyInterface * body0, RigidBodyInterface * body1, float x, float y, float z) override;
private:
	NxPhysicsSDK * m_physicsSDK;
	NxScene		 * m_scene;
};