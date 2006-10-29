#include "RigidBody.h"
#include "float.h"
//////////////////////////////////////////////////////////////////////////
RigidBody::RigidBody(CollisionShape* _shape, mt::vec2f _pos, float _rot, float _m, float _I, long int _id, void* _ud)
{
	userData = _ud;
	shape = _shape;
	dead = false;
	sleeptime = 0.0f;
	uniqueId = _id;

	prevx = mt::vec2f(0.0f, 0.0f);
	prevrot = 0.0f;
	
	x = _pos;
	v = mt::vec2f(0.0f, 0.0f);
	F = mt::vec2f(0.0f, 0.0f);

	T = 0.0f;
	rot = _rot;
	w = 0.0f;

	nu = 0.2f;
	
	if (_m < FLT_MAX)
	{
		m_1 = 1.0f / _m;
		I_1 = 1.0f / _I;
		state = DYNAMIC;
	}
	else
	{
		m_1 = 0.0f;
		I_1 = 0.0f;
		state = STATIC;
	}

	shape->evaluteBBox(0);
}
//////////////////////////////////////////////////////////////////////////
RigidBody::~RigidBody()
{}
//////////////////////////////////////////////////////////////////////////
void	RigidBody::sleep()
{
	if (state != STATIC)
	{
		dead = true;
	}
}
//////////////////////////////////////////////////////////////////////////
void	RigidBody::wake()
{
	if (!dead)
	{
		return;
	}

	dead = false;
	sleeptime = 0.0f;

	v = mt::vec2f(0.0f, 0.0f);
	w = 0.0f;

	F = mt::vec2f(0.0f, 0.0f);
	T = 0.0f;
}
//////////////////////////////////////////////////////////////////////////
void	RigidBody::integrate(float dt)
{
	if (!dead)
	{
		x += v * dt;
		rot += w * dt;

		if ((mt::vec2f(fabs(x.x - prevx.x), fabs(x.y - prevx.y))).length() 
			< epsSleepVelocity && fabs(rot - prevrot) < epsSleepRotMov)
		{
			sleeptime += dt;
		}
		else
		{
			sleeptime = 0.0f;
		}

		if (sleeptime > 1.0f)
		{
			sleep();
		}
	}

	prevx.x = x.x;
	prevx.y = x.y;
	prevrot = rot;

	F = mt::vec2f(0.0f, 0.0f);
	T = 0.0f;
}

void* RigidBody::getUserData() const
{
	return	userData;
}

CollisionShape* RigidBody::getShape() const
{
	return shape;
}

long int RigidBody::getID() const
{
	return uniqueId;
}

eCollisionFlags RigidBody::getState() const
{
	return state;
}

bool RigidBody::isDead() const
{
	return dead;
}

bool RigidBody::isSleepTimeIsZero() const
{
	return sleeptime == 0.0f;
}