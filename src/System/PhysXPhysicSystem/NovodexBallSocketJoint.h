#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include "NovodexBody.h"

class	NovodexBallSocketJoint : public Menge::BallSocketJointInterface
{
public:
	NovodexBallSocketJoint(NxScene * _scene)
		: m_SJoint(0)
		, m_SJdesc(0)
		, m_scene(_scene)
	{}

	~NovodexBallSocketJoint()
	{
		m_scene->releaseJoint( *m_SJoint );
		delete m_SJdesc;
	}

	void init(
		Menge::RigidBodyInterface * body0,
		Menge::RigidBodyInterface * body1,
		float x, float y, float z)
	{
		m_SJdesc = new NxSphericalJointDesc();

		NovodexBody * novodexBody0 = dynamic_cast<NovodexBody*>(body0);
		NovodexBody * novodexBody1 = dynamic_cast<NovodexBody*>(body1);
		
		NxVec3 pivot(x,y,z);

		m_SJdesc->setToDefault();
		m_SJdesc->actor[0] = novodexBody0->getNxActor();
		m_SJdesc->actor[1] = novodexBody1->getNxActor();
		m_SJdesc->setGlobalAnchor(pivot);

		NxJoint * joint = m_scene->createJoint(*m_SJdesc);

		m_SJoint = joint->isSphericalJoint();

		if (joint == NULL)
		{
			assert(0);
			return;
		}
	}

	void setLimits(float cone_limit_rad, float twist_limit_rad) 
	{		
		m_SJdesc->twistLimit.low.value = -twist_limit_rad;
		m_SJdesc->twistLimit.high.value = twist_limit_rad;
		m_SJdesc->twistSpring.setToDefault();

		m_SJdesc->swingLimit.value = cone_limit_rad;
		m_SJdesc->swingSpring.setToDefault();

		m_SJoint->loadFromDesc(*m_SJdesc);
		m_SJoint->setFlags( m_SJoint->getFlags() | NX_SJF_TWIST_LIMIT_ENABLED | NX_SJF_SWING_LIMIT_ENABLED );
	}


private:
	NxSphericalJoint  * m_SJoint;
	NxSphericalJointDesc * m_SJdesc;

	NxScene	*	m_scene;
};