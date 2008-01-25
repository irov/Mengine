#	pragma once

#	include <NxPhysics.h>

#	include "Interface/PhysicSystemInterface.h"

class NovodexBody : public RigidBodyInterface
{
public:
	NovodexBody(NxActor * _actor);
	~NovodexBody();

public:
	void setOrient( float _w, float _x, float _y, float _z );
	
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
public:
	NxActor * getNxActor() const;

private:
	NxActor *	m_actor;
	NxVec3  m_position;
	NxVec3  m_dir;
	float   m_quat[4];
	float	m_rot[9];
};
