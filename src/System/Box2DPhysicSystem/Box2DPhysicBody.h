#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include "Box2D/Box2D.h"

namespace Menge
{
	class Box2DPhysicBody
		: public PhysicBodyInterface
	{
	public:
		Box2DPhysicBody();

	public:
		void initialize( b2Body * _body );

	public:
		void setTransform( const mt::vec2f & _pos, float _angle ) override;
		void setAngle( float _angle ) override;
		void setLinearVelocity( const mt::vec2f & _linearVelocity ) override;
		void setAngularVelocity( float _angularVelocity ) override;
		void setLinearDamping( float _linearDamping ) override;
		void setAngularDamping( float _angularDamping ) override;
		void setAllowSleep( bool _allowSleep ) override;
		void setAwake( bool _awake ) override;
		void setFixedRotation( bool _fixedRotation ) override;
		void setBullet( bool _bullet ) override;
		void setActive( bool _active ) override;

	public:
		void createConvex( PhysicShapeConvex * _convex, PhysicFixture * _fixture, bool isSensor ) override;
		void createCircle( PhysicShapeCircle * _convex, PhysicFixture * _fixture, bool isSensor ) override;
		void createBox( PhysicShapeBox * _convex, PhysicFixture * _fixture, bool isSensor ) override;
		void createEdge( PhysicShapeEdge * _convex, PhysicFixture * _fixture, bool isSensor ) override;
		void createChain( PhysicShapeChain * _convex, PhysicFixture * _fixture, bool isSensor ) override;

	protected:
		b2Body * m_body;
	};
}