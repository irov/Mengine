#include "PhysSystem.h"

bool initInterfaceSystem(PhysicSystemInterface **_system)
{
	try
	{
		*_system = new PhysEngine();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void releaseInterfaceSystem(PhysicSystemInterface* _system)
{
	delete static_cast<PhysEngine*>(_system);
}

PhysEngine::PhysEngine()
{
	m_CollisionDispatcher = NULL;
	m_BroadPhaseCollisionDetector = NULL;
	iterations = 11;
	gravity = mt::vec2f(0.0f,-9.8f);
	numBodies = 0;
	numJoints = 0;
	m_callbackFunction = NULL;
}

void PhysEngine::loadParams(const PhysParamSimDesc& _desc)
{
	dt = _desc.dtvalue;
	iterations = _desc.iterval;
	gravity = _desc.g;
	m_CollisionDispatcher = new CollisionAlgorithmDispatcher();
	m_CollisionDispatcher->Init();
	m_BroadPhaseCollisionDetector = new LinearTimeBroadphaser(10,10);
	m_BroadPhaseCollisionDetector->Init();
}

CollisionShape* PhysEngine::createBoxShape(const mt::vec2f& width)
{
	return	new BoxShape(width);
}

CollisionShape* PhysEngine::createCircleShape(float R)
{
	return	new CircleShape(R);
}

void PhysEngine::removeBoxShape(CollisionShape* _bs)
{
	delete static_cast<BoxShape*>(_bs);
}

void PhysEngine::removeCircleShape(CollisionShape* _cs)
{
	delete static_cast<CircleShape*>(_cs);
}

void PhysEngine::setUserDataCallback(callbackOnChangePivot* callback)
{
	m_callbackFunction = callback;
}

PhysEngine::~PhysEngine()
{
	removeObjects();
	delete m_CollisionDispatcher;
	delete m_BroadPhaseCollisionDetector;
}

void PhysEngine::setGravity(const mt::vec2f& _g)
{
	gravity = _g;
}

mt::vec2f PhysEngine::getGravity() const
{
	return	gravity;
}

RigidBodyInterface* PhysEngine::addRigidBody(CollisionShape* _b, mt::vec2f _pos, float _rot, float _mass, float _inertia, void* _userData)
{
	bodies[numBodies] = new RigidBody(_b,_pos,_rot,_mass,_inertia,numBodies,_userData);
	numBodies++;
	return bodies[numBodies-1];
}

JointInterface* PhysEngine::addJoint(RigidBodyInterface* _b1, RigidBodyInterface* _b2, mt::vec2f _anchor)
{
	joints[numJoints] = new ElasticJoint(_b1,_b2,_anchor);
	numJoints++;
	return joints[numJoints-1];
}

void PhysEngine::removeObjects()
{
	for (int i = 0; i < numBodies; i++)
	{
		delete (RigidBody*)bodies[i];
	}
	numBodies = 0;
	for (int i = 0; i < numJoints; i++)
	{
		delete joints[i];
	}
	numJoints = 0;
}

float PhysEngine::getDeltaTime() const
{
	return dt;
}

float PhysEngine::getFrequency() const
{
	return 1.0f/dt;
}

void PhysEngine::integrate()
{
	int	numPairContacts = 0;

	m_BroadPhaseCollisionDetector->DoBroadphase(m_CollisionDispatcher,
		bodies,
		numBodies,
		collisionPairs,
		numPairContacts);

	const float	inv_dt = 1.0f/dt;

	for (int i = 0; i < numBodies; ++i)
	{
		RigidBodyInterface* b = bodies[i];

		if (b->m_1 == 0.0f || b->isDead())
		{
			continue;
		}

		b->v += (gravity + b->F * b->m_1) * dt;
		b->w += dt * b->I_1 * b->T;
	}

	for (int j = 0; j < numPairContacts; ++j)
	{
		CollisionPair* collPair	= &collisionPairs[j];

		RigidBody* b1 = (RigidBody*)collPair->body1;
		RigidBody* b2 = (RigidBody*)collPair->body2;

		if (b1->isSleepTimeIsZero() || b2->isSleepTimeIsZero())
		{
			b1->wake();
			b2->wake();
		}
		else if (b1->isDead())
		{
			b2->sleep();
		}
		else if (b2->isDead())
		{
			b1->sleep();
		}

		for (int i = 0; i < collPair->numContacts; ++i)
		{
			Contact* c = &collPair->contacts[i];

			mt::vec2f r1 = c->pos - b1->x;
			mt::vec2f r2 = c->pos - b2->x;

			float	rn1 = r1.x * c->normal.x + r1.y * c->normal.y;
			float	rn2 = r2.x * c->normal.x + r2.y * c->normal.y;
			
			float	dr1r2 = r1.x * r1.x + r1.y * r1.y;
			float	dr2r2 = r2.x * r2.x + r2.y * r2.y;

			c->massNormal = 
				1.0f / (b1->m_1 + b2->m_1 + b1->I_1 * (dr1r2 - rn1 * rn1) + b2->I_1 * (dr2r2 - rn2 * rn2));

			mt::vec2f tangent(c->normal.y,-c->normal.x);

			float rt1 = dot_v2_v2(r1, tangent);
			float rt2 = dot_v2_v2(r2, tangent);
			
			c->massTangent =
				1.0f /
				(b1->m_1 + b2->m_1 + b1->I_1 * (dr1r2 - rt1 * rt1) + b2->I_1 * (dr2r2 - rt2 * rt2));

			c->bias = -0.1f * inv_dt * std::min(0.0f, c->pdepth);
		}
	}

	for (int i = 0; i < numJoints; ++i)
	{
		joints[i]->calculate(inv_dt);
	}

	for (int i = 0; i < iterations; ++i)
	{
		for (int j = 0; j < numPairContacts; ++j)
		{
			CollisionPair* collPair	= &collisionPairs[j];
			RigidBody* b1 = (RigidBody*)collPair->body1;
			RigidBody* b2 = (RigidBody*)collPair->body2;
 
			if (b1->isSleepTimeIsZero() || b2->isSleepTimeIsZero())
			{
				b1->wake();
				b2->wake();
			}

			for (int i = 0; i < collPair->numContacts; ++i)
			{
				Contact* c = &collPair->contacts[i];

				mt::vec2f r1 = c->pos - b1->x;
				mt::vec2f r2 = c->pos - b2->x;
				mt::vec2f dv = b2->v + mt::perp(r2) * b2->w - b1->v - mt::perp(r1) * b1->w;

				float vn = dot_v2_v2(dv, c->normal);
				float normalImpulse = c->massNormal * (c->bias - vn);

				float oldNormalImpulse = c->accNormalImpulse;
				c->accNormalImpulse = std::max(oldNormalImpulse + normalImpulse, 0.0f);
				normalImpulse = c->accNormalImpulse - oldNormalImpulse;

				mt::vec2f impulse = c->normal * normalImpulse;

				b1->v -= impulse * b1->m_1;
				b1->w -= mt::pseudo_cross_v2(r1, impulse) * b1->I_1;

				b2->v += impulse * b2->m_1;
				b2->w += mt::pseudo_cross_v2(r2, impulse) * b2->I_1;

				dv = b2->v + mt::perp(r2) * b2->w - b1->v - mt::perp(r1) * b1->w;

				float maxTangentImpulse = c->accNormalImpulse * b1->nu * b2->nu;

				mt::vec2f tangent(c->normal.y,-c->normal.x);

				float tangentImpulse = -c->massTangent * (dv.x * tangent.x + dv.y * tangent.y);

				float oldTangentImpulse = c->accTangentImpulse;
				c->accTangentImpulse = std::max(-maxTangentImpulse,std::min(oldTangentImpulse + tangentImpulse,maxTangentImpulse));
				tangentImpulse = c->accTangentImpulse - oldTangentImpulse;

				impulse = tangent * tangentImpulse;

				b1->v -= impulse * b1->m_1;
				b1->w -= mt::pseudo_cross_v2(r1, impulse) * b1->I_1;

				b2->v += impulse * b2->m_1;
				b2->w += mt::pseudo_cross_v2(r2, impulse) * b2->I_1;
			}
		}

		for (int i = 0; i < numJoints; ++i)
		{
			joints[i]->integrate();
		}
	}

	for (int i = 0; i < numBodies; ++i)
	{
		bodies[i]->integrate(dt);
//		printf("x = %f; y = %f \n",bodies[i]->x.x,bodies[i]->x.y);
		if(m_callbackFunction)
		{
			m_callbackFunction(bodies[i]);
		}
	}

	//printf("c = %d; n = %d \n",numPairContacts,numBodies);
}