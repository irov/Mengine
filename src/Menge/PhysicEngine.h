#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/PhysicSystemInterface.h"

#	include	<string>

namespace Menge
{
	class PhysicEngine
	{
	public:
		PhysicEngine( PhysicSystemInterface * _interface );
		~PhysicEngine();

	public:

		void	init( const mt::vec3f& g );
		void	update( float _timestep );

		GeometryInterface * cookConvex( const float * _verts, int _vertexSize );
		GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize );
		GeometryInterface * cookConvex( const std::string& _filename );
		GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize );
		GeometryInterface * cookConcave( const std::string& _filename );
		GeometryInterface * cookBox( float _width, float _height, float _depth );

		RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry);
		void	removeRigidBody( RigidBodyInterface * _rigidBody );

	protected:
		PhysicSystemInterface * m_interface;
	};
};