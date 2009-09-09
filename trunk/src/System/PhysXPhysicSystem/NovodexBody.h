#	pragma once

#	include <NxPhysics.h>

#	include "Interface/PhysicSystemInterface.h"

class NovodexBody : public Menge::RigidBodyInterface
{
public:
	NovodexBody(NxActor * _actor);
	~NovodexBody();

public:
	float * getPosition() override;
	float * getOrient() override;
		
	void setActive( bool _active ) override;
	void applyImpulse( float _x, float _y, float _z ) override;
	void applyForce( float _x, float _y, float _z ) override;
	void applyAngularImpulse(float _x, float _y, float _z ) override;
	void applyTorque( float _x, float _y, float _z ) override;
	void setLinearVelocity( float _x, float _y, float _z ) override;
	void setAngularVelocity( float _x, float _y, float _z ) override;
	void setPosition(float _x, float _y, float _z) override;
	void setOrient( float _w, float _x, float _y, float _z ) override;

	bool isSleeping() const override;
public:
	NxActor * getNxActor() const;

private:
	NxActor * m_actor;
};