#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"
#	include "Box2D.h"

class Box2DPhysicBody
	: public PhysicBody2DInterface
{
public:
	Box2DPhysicBody( b2World* _world, const b2BodyDef& _bodyDef );
	virtual ~Box2DPhysicBody();

	const float * getPosition() override;
	const float * getOrientation() override;

	void setLinearVelocity( float x, float y ) override;
	const float* getLinearVelocity() override;
	void setAngularVelocity( float w ) override;
	float getAngularVelocity() override;
	void applyForce( float forceX, float forceY, float pointX, float pointY ) override;

private:
	b2World* m_world;
	b2Body* m_body;
};