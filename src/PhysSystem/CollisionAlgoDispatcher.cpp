#include "CollisionAlgoDispatcher.h"
//////////////////////////////////////////////////////////////////////////
void CollisionAlgorithmDispatcher::Init()
{
	for(int i = 0; i < MAX_COLL_FUN; i++)
	{
		for(int j = 0; j < MAX_COLL_FUN; j++)
		{
			colliders[i][j] = 0;
		}
	}

	SetCollider(BOX,BOX,CollideBoxBox);
	SetCollider(BOX,CIRCLE,CollideBoxCircle);
	SetCollider(CIRCLE,CIRCLE,CollideCircleCircle);
}
//////////////////////////////////////////////////////////////////////////
void CollisionAlgorithmDispatcher::SetCollider(int body1, int body2, CollisionAlgorithm* fn) 
{
	colliders[body1][body2] = fn;
};
//////////////////////////////////////////////////////////////////////////
CollisionAlgorithm* CollisionAlgorithmDispatcher::GetCollider(int body1, int body2) const
{
	return	colliders[body1][body2];
}
//////////////////////////////////////////////////////////////////////////