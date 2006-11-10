#pragma once

#include	"interfaces.h"

#include	"Broadphaser.h"

#include	"CollisionAlgoDispatcher.h"

#include	"RigidBody.h"

#include	"Joint.h"

class PhysEngine : public PhysicSystemInterface
{
public:
	PhysEngine();
	~PhysEngine();
public:
	/*	operation on shapes	*/
	CollisionShape* createBoxShape(const mt::vec2f& width);
	CollisionShape* createCircleShape(float R);
	void removeBoxShape(CollisionShape* _bs);
	void removeCircleShape(CollisionShape* _cs);
	
	/*	callback to user function, called in final step of integrator:	*/
	void setUserDataCallback(callbackOnChangePivot* callback);

	/*	loading params:	*/
	void loadParams(const PhysParamSimDesc& _desc);

	/*	operation on rigid body:	*/
	RigidBodyInterface* addRigidBody(CollisionShape* _b, mt::vec2f _pos, float _rot, float _mass, float _inertia, void* _userData);
	JointInterface* addJoint(RigidBodyInterface* _b1, RigidBodyInterface* _b2, mt::vec2f _anchor);


	void integrate();
	void removeObjects();
	void setGravity(const mt::vec2f& _g);
	mt::vec2f getGravity() const;
	float getDeltaTime() const;
	float getFrequency() const;

	enum{MAX_BODIES = 500};
	enum{MAX_JOINTS = 500};
	RigidBodyInterface* bodies[MAX_BODIES];
	JointInterface* joints[MAX_JOINTS];
private:
/*	all physics engine's main params:	*/
	mt::vec2f gravity;
	long int iterations;
	float dt;

/*	user callback function:	*/
	callbackOnChangePivot* m_callbackFunction;

/*	other physic systems:	*/
	CollisionAlgorithmDispatcher* m_CollisionDispatcher;
	Broadphaser* m_BroadPhaseCollisionDetector;

/*	pair contacts in system:	*/
	enum{MAX_PAIR_CONTACTS = 1000};
	CollisionPair collisionPairs[MAX_PAIR_CONTACTS];
public:
/*	number of bodies:	*/
	int	numBodies;

/*	number of joints:	*/
	int numJoints;
};