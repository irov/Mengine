#	include "PhysicEngine.h"
#	include "FileEngine.h"
#	include "Logger/Logger.h"

namespace Menge
{
	PhysicEngine::PhysicEngine( PhysicSystemInterface * _interface )
	: m_interface( _interface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine::~PhysicEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	PhysicEngine::init( const mt::vec3f& g )
	{
		return m_interface->init(g.x, g.y, g.z);
	}
	//////////////////////////////////////////////////////////////////////////
	void	PhysicEngine::update( float _timestep )
	{
		return m_interface->update( _timestep );
	}
	//////////////////////////////////////////////////////////////////////////
	ControllerInterface * PhysicEngine::createCapsuleController( const mt::vec3f & _startPos, float _initialRadius, float _initialHeight )
	{
		return m_interface->createCapsuleController( _startPos.buff(), _initialRadius, _initialHeight );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConvex( const float * _verts, int _vertexSize )
	{
		return m_interface->cookConvex( _verts, _vertexSize );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
	{
		return m_interface->cookConvex( _verts, _vertexSize, _indecies, _indexSize );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConvex( const String& _filename )
	{
		return m_interface->cookConvex( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
	{
		return m_interface->cookConcave( _verts, _vertexSize, _indecies, _indexSize );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConcave( const String& _filename )
	{
		return m_interface->cookConcave( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookBox( float _width, float _height, float _depth )
	{
		return m_interface->cookBox( _width, _height, _depth  );
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBodyInterface * PhysicEngine::createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry)
	{
		return m_interface->createRigidBody( _density, _dynamic, _geometry );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::removeRigidBody( RigidBodyInterface * _rigidBody )
	{
		return m_interface->removeRigidBody( _rigidBody );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setRestitution( float _value )
	{
		return m_interface->setRestitution(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setStaticFriction( float _value )
	{
		return m_interface->setStaticFriction(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setDynamicFriction( float _value )
	{
		return m_interface->setDynamicFriction(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setGravity( const mt::vec3f& _g )
	{
		return m_interface->setGravity(_g.x, _g.y, _g.z);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & PhysicEngine::getGravity() const
	{
		return *(mt::vec3f*)m_interface->getGravity();
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::removeGeometry( GeometryInterface * _geom )
	{
		return m_interface->removeGeometry(_geom);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::releaseCapsuleController( ControllerInterface * _capsule )
	{
		return m_interface->releaseCapsuleController( _capsule );
	}
	//////////////////////////////////////////////////////////////////////////
}
