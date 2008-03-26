#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"
#	include "Box2DPhysicSystem.h"

class Box2DPhysicJoint
	: public PhysicJoint2DInterface
{
public:
	Box2DPhysicJoint( b2World* _world, b2Joint* _joint );
	virtual ~Box2DPhysicJoint();

	void _setJoint( b2Joint* _joint );
private:
	b2World* m_world;
	b2Joint* m_joint;
};