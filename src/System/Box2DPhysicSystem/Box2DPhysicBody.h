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

	void setLinearVelocity( float _x, float _y ) override;
	const float* getLinearVelocity() override;
	void setAngularVelocity( float _w ) override;
	float getAngularVelocity() override;
	void applyForce( float _forceX, float _forceY, float _pointX, float _pointY ) override;
	void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY ) override;

	void setCollisionListener( PhysicBody2DCollisionListener* _listener ) override;
	void setUserData( void* _data ) override;
	void* getUserData() const override;

	bool isFrozen() const override;
	bool isSleeping() const override;
	bool isStatic() const override;

	void wakeUp() override;

	b2Body* getBody();

private:
	b2World* m_world;
	b2Body* m_body;
	PhysicBody2DCollisionListener* m_listener;
	void* m_userData;

	friend void Box2DPhysicSystem::update( float _timing, int _iterations );
	void _collide( b2Body* _otherBody, b2Contact* _contact );
};