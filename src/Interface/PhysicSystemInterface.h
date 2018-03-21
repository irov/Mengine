#pragma once

#include "Interface/ServiceInterface.h"

#include "Core/ConstString.h"

#include "math/vec2.h"

namespace Mengine
{
	class PhysicFixture
	{
		float friction;
		float restitution;
		float density;
	};

	class PhysicShapeConvex
	{
		mt::vec2f p[8];
		uint32_t count;
	};

	class PhysicShapeCircle
	{
		float radius;
	};

	class PhysicShapeBox
	{
		float width;
		float height;
	};

	class PhysicShapeEdge
	{
		float width;
		float height;
	};

	class PhysicShapeChain
	{
		mt::vec2f * p;
		uint32_t count;

		bool loop;
	};

	class PhysicBodyInterface
	{
	public:
		virtual void setTransform( const mt::vec2f & _pos, float _angle ) = 0;
		virtual void setAngle( float _angle ) = 0;
		virtual void setLinearVelocity( const mt::vec2f & _linearVelocity ) = 0;
		virtual void setAngularVelocity( float _angularVelocity ) = 0;
		virtual void setLinearDamping( float _linearDamping ) = 0;
		virtual void setAngularDamping( float _angularDamping ) = 0;
		virtual void setAllowSleep( bool _allowSleep ) = 0;
		virtual void setAwake( bool _awake ) = 0;
		virtual void setFixedRotation( bool _fixedRotation ) = 0;
		virtual void setBullet( bool _bullet ) = 0;
		virtual void setActive( bool _active ) = 0;

	public:
		virtual void createConvex( PhysicShapeConvex * _convex, PhysicFixture * _fixture, bool isSensor ) = 0;
		virtual void createCircle( PhysicShapeCircle * _convex, PhysicFixture * _fixture, bool isSensor ) = 0;
		virtual void createBox( PhysicShapeBox * _convex, PhysicFixture * _fixture, bool isSensor ) = 0;
		virtual void createEdge( PhysicShapeEdge * _convex, PhysicFixture * _fixture, bool isSensor ) = 0;
		virtual void createChain( PhysicShapeChain * _convex, PhysicFixture * _fixture, bool isSensor ) = 0;
	};

	enum EPBType
	{
		EPBT_STATIC,
		EPBT_DYNAMIC,
		EPBT_KINEMATIC
	};

	class PhysicWorldInterface
	{
		virtual PhysicBodyInterface * createBody( EPBType _type ) = 0;

		virtual void destroyBody( PhysicBodyInterface * _body ) = 0;
	};

	class PhysicSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PhysicSystem");

	public:
		virtual PhysicWorldInterface * createWorld( const ConstString & _name, float _gravityX, float _gravityY ) = 0;
		virtual void destroyWorld( const ConstString & _name ) = 0;

	public:
		virtual void update( float _timing, int _velocityIterations, int _positionIterations ) = 0;

	
	};

#   define PHYSIC_SYSTEM()\
    SERVICE_GET(Mengine::PhysicSystemInterface)
}