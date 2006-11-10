#pragma once
#include "math/vec2.h"
//////////////////////////////////////////////////////////////////////////
const mt::vec2f	mars_g(0.0f,-3.69f);
const mt::vec2f	mercury_g(0.0f,-3.701f);
const mt::vec2f	moon_g(0.0f,-1.63f);
const mt::vec2f	jupiter_g(0.0f,-23.12f);
const mt::vec2f	saturn_g(0.0f,-8.96f);
const mt::vec2f	pluto_g(0.0f,-0.58f);
const mt::vec2f	earth_g(0.0f,-9.81f);
//////////////////////////////////////////////////////////////////////////
struct PhysParamSimDesc 
{
	/*	gravity in world */
	mt::vec2f g;
	/*	delta time, by default is 1/60.0f */
	float dtvalue;
	/*	preferred fps value	*/
	float fpstarget;
	/*	number of iterations, by default is 10 */
	int iterval;
};
//////////////////////////////////////////////////////////////////////////
enum eCollisionFlags
{
	STATIC = 1,
	DYNAMIC = 2,
};
//////////////////////////////////////////////////////////////////////////
class	CollisionShape
{
public:
	virtual	int		getShapeType() const = 0;
	virtual void	evaluteBBox(float _rot) = 0;
	virtual	const	mt::vec2f&	getBBoxInfo() const = 0;
};
//////////////////////////////////////////////////////////////////////////
class RigidBodyInterface
{
public:
	virtual CollisionShape* getShape() const = 0;
	virtual eCollisionFlags getState() const = 0;
	virtual long int getID() const = 0;
	virtual bool isDead() const = 0;
	virtual bool isSleepTimeIsZero() const = 0;
	virtual void integrate(float dt) = 0;
	virtual void sleep() = 0;
	virtual void wake() = 0;
	virtual void* getUserData() const = 0;
public: 
	mt::vec2f x;
	mt::vec2f v;
	mt::vec2f F;
	float rot;
	float w;
	float T;
	float I_1;
	float m_1;
	float nu;
};
//////////////////////////////////////////////////////////////////////////
typedef void callbackOnChangePivot(RigidBodyInterface* rb);
//////////////////////////////////////////////////////////////////////////
class JointInterface
{
public:
	virtual void calculate(float inv_dt) = 0;
	virtual void integrate() = 0;
	RigidBodyInterface* body1;
	RigidBodyInterface* body2;
};
//////////////////////////////////////////////////////////////////////////
class PhysicSystemInterface
{
public:
	virtual CollisionShape* createBoxShape(const mt::vec2f& width) = 0;
	virtual CollisionShape* createCircleShape(float R) = 0;
	virtual void removeBoxShape(CollisionShape* _bs) = 0;
	virtual void removeCircleShape(CollisionShape* _cs) = 0;
	virtual RigidBodyInterface* addRigidBody(CollisionShape* _b, mt::vec2f _pos, float _rot, float _mass, float _inertia, void* _userData) = 0;
	virtual JointInterface* addJoint(RigidBodyInterface* _b1, RigidBodyInterface* _b2, mt::vec2f _anchor) = 0;
	virtual	void setUserDataCallback(callbackOnChangePivot* callback) = 0;
	virtual void loadParams(const PhysParamSimDesc& _desc) = 0;
	virtual void integrate() = 0;
	virtual void removeObjects() = 0;
	virtual void setGravity(const mt::vec2f& _g) = 0;
	virtual mt::vec2f getGravity() const = 0;
	virtual float getDeltaTime() const = 0;
	virtual float getFrequency() const = 0;
};
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(PhysicSystemInterface**);
void releaseInterfaceSystem(PhysicSystemInterface*);
//////////////////////////////////////////////////////////////////////////