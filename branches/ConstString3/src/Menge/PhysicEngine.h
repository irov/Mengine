//#	pragma once
//
//#	include "Core/Holder.h"
//#	include	"math/vec3.h"
//
//#	include "Interface/PhysicSystemInterface.h"
//
//#	include	<string>
//
//namespace Menge
//{
//	class PhysicEngine
//		: public Holder<PhysicEngine>
//	{
//	public:
//		PhysicEngine( PhysicSystemInterface * _interface );
//		~PhysicEngine();
//
//	public:
//		void init( const mt::vec3f& g );
//		void update( float _timestep );
//
//	public:
//		void setGravity( const mt::vec3f& _g );
//		void setRestitution( float _value );
//		void setStaticFriction( float _value );
//		void setDynamicFriction( float _value );
//
//		const mt::vec3f & getGravity() const;
//
//	public:
//		ControllerInterface * createCapsuleController( const mt::vec3f & _startPos, float _initialRadius, float _initialHeight );
//		
//	public:
//		GeometryInterface * cookConvex( const float * _verts, int _vertexSize );
//		GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize );
//		GeometryInterface * cookConvex( const ConstString& _filename );
//		GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize );
//		GeometryInterface * cookConcave( const ConstString& _filename );
//		GeometryInterface * cookBox( float _width, float _height, float _depth );
//
//		void removeGeometry( GeometryInterface * _geom );
//		void releaseCapsuleController( ControllerInterface * _capsule );
//
//	public:
//		RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry);
//		void	removeRigidBody( RigidBodyInterface * _rigidBody );
//	protected:
//		PhysicSystemInterface * m_interface;
//	};
//};
