#pragma once
#include "interfaces.h"
//////////////////////////////////////////////////////////////////////////
const float epsSleepVelocity = 0.008f;
const float epsSleepRotMov = 0.008f;
//////////////////////////////////////////////////////////////////////////
class RigidBody : public RigidBodyInterface
{
public:
	RigidBody(CollisionShape* _shape, mt::vec2f, float _rot, float _m, float _I, long int _id, void* _ud = 0);
	~RigidBody();
public:
	void integrate(float dt);
	void sleep();
	void wake();
	void* getUserData() const;
	CollisionShape* getShape() const;
	eCollisionFlags getState() const;
	long int getID() const;
	bool isDead() const;
	bool isSleepTimeIsZero() const;
private:
	int	uniqueId;
	void* userData;
	CollisionShape* shape;
	eCollisionFlags state;
	bool dead;
	float sleeptime;
	mt::vec2f prevx;
	float prevrot;
};