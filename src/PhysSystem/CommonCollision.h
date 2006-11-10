#pragma once

#include	"interfaces.h"

#include	"math\vec2.h"

//////////////////////////////////////////////////////////////////////////
struct Contact
{
	mt::vec2f pos;
	mt::vec2f normal;
	float pdepth;
	float accNormalImpulse;
	float accTangentImpulse;
	float massNormal;
	float massTangent;
	float bias;
};
//////////////////////////////////////////////////////////////////////////
struct CollisionPair
{
	enum{MAX_COLL_CONTACTS = 2};
	Contact	contacts[MAX_COLL_CONTACTS];
	int	numContacts;
	RigidBodyInterface* body1;
	RigidBodyInterface* body2;
};