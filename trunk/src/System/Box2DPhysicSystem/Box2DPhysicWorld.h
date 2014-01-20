#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include "Box2DPhysicBody.h"

#	include "Box2D/Box2D.h"


namespace Menge
{
	class Box2DPhysicWorld
		: public PhysicWorldInterface
		, public b2ContactListener
	{
	public:
		Box2DPhysicWorld();

	public:
		bool initialize( float _gravityX, float _gravityY );
		void finalize();

	public:
		PhysicBodyInterface * createBody( EPBType _type ) override;

		void destroyBody( PhysicBodyInterface * _body ) override;

	public:
		void step( float _timing, int _velocityIterations, int _positionIterations );

	protected:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

		void PreSolve( b2Contact* contact, const b2Manifold* oldManifold ) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

	protected:
		b2World * m_world;
	};
}