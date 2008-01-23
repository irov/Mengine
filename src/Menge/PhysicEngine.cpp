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
	RigidBodyInterface * PhysicEngine::createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry)
	{
		return m_interface->createRigidBody( _density, _dynamic, _geometry );
	}
	//////////////////////////////////////////////////////////////////////////
	void	PhysicEngine::removeRigidBody( RigidBodyInterface * _rigidBody )
	{
		return m_interface->removeRigidBody( _rigidBody );
	}
	//////////////////////////////////////////////////////////////////////////
}