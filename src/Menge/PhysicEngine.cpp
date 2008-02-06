#	include "PhysicEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"

namespace Menge
{
	PhysicEngine::PhysicEngine( PhysicSystemInterface * _interface )
	: m_interface( _interface )
	{
		Holder<PhysicEngine>::keep( this );
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
	const mt::vec3f & PhysicEngine::getControllerPos()
	{
		return *(mt::vec3f*)m_interface->getControllerPos();
	}
	//////////////////////////////////////////////////////////////////////////
	void	PhysicEngine::moveController( const mt::vec3f & _dispvec )
	{
		m_interface->moveController( (float*)_dispvec.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	PhysicEngine::createController( const mt::vec3f & _startPos, float _initialRadius, float _initialHeight )
	{
		m_interface->createController( (float*)_startPos.m, _initialRadius, _initialHeight );
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
	GeometryInterface * PhysicEngine::cookConvex( const std::string& _filename )
	{
		return m_interface->cookConvex( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
	{
		return m_interface->cookConcave( _verts, _vertexSize, _indecies, _indexSize );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookConcave( const std::string& _filename )
	{
		return m_interface->cookConcave( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	GeometryInterface * PhysicEngine::cookBox( float _width, float _height, float _depth )
	{
		return m_interface->cookBox( _width, _height, _depth  );
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicEngine::rayCast( const std::string & _name, const mt::vec3f & _pos,  const mt::vec3f & _dir )
	{
		return m_interface->rayCast( _name.c_str(), (float*)_pos.m, (float*)_dir.m );
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
		m_interface->setRestitution(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setStaticFriction( float _value )
	{
		m_interface->setStaticFriction(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setDynamicFriction( float _value )
	{
		m_interface->setDynamicFriction(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::setGravity( const mt::vec3f& _g )
	{
		m_interface->setGravity(_g.x, _g.y, _g.z);
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine::removeGeometry( GeometryInterface * _geom )
	{
		m_interface->removeGeometry(_geom);
	}
}