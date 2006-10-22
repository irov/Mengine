#pragma once
//////////////////////////////////////////////////////////////////////////
#include "StandartCollisionAlgorithms.h"
//////////////////////////////////////////////////////////////////////////
typedef int CollisionAlgorithm(CollisionPair& cp, RigidBodyInterface* body1, RigidBodyInterface* body2);
//////////////////////////////////////////////////////////////////////////
class CollisionAlgorithmDispatcher 
{
public:
	enum	COLLISION_PRIMITIVES
	{
		STANDART_CD,
		USER_DEFINED_CD,
	};
public:
	void Init(COLLISION_PRIMITIVES _type);
	void SetCollider(int body1, int body2, CollisionAlgorithm* fn);
	CollisionAlgorithm* GetCollider(int body1, int body2) const;
private:
	enum{MAX_COLL_FUN = 10};
	CollisionAlgorithm* colliders[MAX_COLL_FUN][MAX_COLL_FUN];
};
//////////////////////////////////////////////////////////////////////////