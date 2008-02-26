#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"
#	include "Box2D.h"
#	include "Box2DPhysicSystem.h"

class Box2DPhysicBody
	: public PhysicBody2DInterface
{
public:
	Box2DPhysicBody( b2World* _world, const b2BodyDef& _bodyDef );
	virtual ~Box2DPhysicBody();

	const float * getPosition() const override;
	void setPosition( float _x, float _y ) override;
	const float * getOrientation() override;
	void setOrientation( float _angle ) override;

	void setLinearVelocity( float x, float y ) override;
	const float* getLinearVelocity() override;
	void setAngularVelocity( float w ) override;
	float getAngularVelocity() override;
	void applyForce( float forceX, float forceY, float pointX, float pointY ) override;

	void setCollisionListener( PhysicBody2DCollisionListener* _listener ) override;

private:
	b2World* m_world;
	b2Body* m_body;
	PhysicBody2DCollisionListener* m_listener;

	friend void Box2DPhysicSystem::update( float _timing, int _iterations );
	void _collide( b2Body* _otherBody, b2Contact* _contact );
};