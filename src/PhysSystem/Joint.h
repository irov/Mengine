#pragma once
//////////////////////////////////////////////////////////////////////////
#include "interfaces.h"
//////////////////////////////////////////////////////////////////////////
class ElasticJoint
	: public JointInterface
{
public:
	ElasticJoint(RigidBodyInterface* body1, RigidBodyInterface* body2, const mt::vec2f& anchor);
	void calculate(float inv_dt);
	void integrate();
	mt::vec2f localAnchor1;
	mt::vec2f localAnchor2;
private:
	mt::vec2f r1;
	mt::vec2f r2;
	float M[4];
	mt::vec2f bias;
	mt::vec2f accumulatedImpulse;
	float relaxation;
};