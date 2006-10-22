#include "Broadphaser.h"
//////////////////////////////////////////////////////////////////////////
LinearTimeBroadphaser::LinearTimeBroadphaser(int _ratio, int _gridSize)
{
	gridSize = _gridSize;
	ratio = _ratio;
}
//////////////////////////////////////////////////////////////////////////
void	LinearTimeBroadphaser::Init()
{
	worldGrid.resize(ratio * ratio);
	for (int i = 0; i < MAX_BODIES; i++)
	{
		for (int j = 0; j < MAX_BODIES; j++)
		{
			collisions[i][j] = false;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void LinearTimeBroadphaser::DoBroadphase(CollisionAlgorithmDispatcher* CD, RigidBodyInterface** collisionObjects, int numCollisionObjects, CollisionPair* outCollisionPair, int& numCP)
{
	for (int i = 0; i < numCollisionObjects; ++i)
	{
		RigidBodyInterface* co1 = collisionObjects[i];

		if (!co1->isDead()/* && !co1->getShape()->GetShapeType() == CIRCLE*/)
		{
			co1->getShape()->EvaluteBBox(co1->rot);
		}

		mt::vec2f	tempBBox = co1->getShape()->GetBBoxInfo();

		int g1 = (co1->x.x - tempBBox.x*0.5f)/gridSize + ratio * 0.5f;
		int g2 = (co1->x.y - tempBBox.y*0.5f)/gridSize + ratio * 0.5f;
		int g3 = (co1->x.x + tempBBox.x*0.5f)/gridSize + ratio * 0.5f;
		int g4 = (co1->x.y + tempBBox.y*0.5f)/gridSize + ratio * 0.5f;

		for (int j = g1; j <= g3; j++)
			for (int k = g2; k <= g4; k++)
			{
				int g = k * ratio + j;

				if (g < 0)
				{
					g *= -1;
				}

				int sqrRatioWG = ratio * ratio;

				if (g >= sqrRatioWG)
				{
					g = g % sqrRatioWG;
				}

				if (g >= worldGrid.size())
				{
					continue;
				}

				for (int c = 0; c < worldGrid[g].size(); c++)
				{
					RigidBodyInterface* co2 = worldGrid[g][c];

					if (collisions[co1->getID()][co2->getID()])
					{
						continue;
					}

					if (co1->getState() == STATIC && co2->getState() == STATIC)
					{
						continue;
					}

					if (co1->getState() == DYNAMIC && co2->getState() == DYNAMIC)
					{
						if (co1->isDead() && co2->isDead())
						{
							continue;
						}

						if (fabs(co1->x.x - co2->x.x) > 
							(co1->getShape()->GetBBoxInfo().x*0.5f + co2->getShape()->GetBBoxInfo().x*0.5f))
						{
							continue;
						}

						if (fabs(co1->x.y - co2->x.y) > 
							(co1->getShape()->GetBBoxInfo().y*0.5f + co2->getShape()->GetBBoxInfo().y*0.5f))
						{
							continue;
						}
					}
					else
					{
						if (co1->isDead() || co2->isDead())
						{
							continue;
						}
					}
					collisionStack.push(CollisionObjectsPair(co1->getID(),co2->getID()));
					collisions[co1->getID()][co2->getID()] = true;
					collisions[co2->getID()][co1->getID()] = true;
				}
				worldGrid[g].push_back(co1);
			}
		}

		while (!collisionStack.empty())
		{
			CollisionObjectsPair p = collisionStack.top();
			collisionStack.pop();
			collisions[p.bodyIndex1][p.bodyIndex2] = false;
			collisions[p.bodyIndex2][p.bodyIndex1] = false;

			CollisionAlgorithm*	cf = NULL;
			int bb1 = p.bodyIndex1;
			int bb2 = p.bodyIndex2;

			bool is_reversed = false;

			cf = CD->GetCollider(
				collisionObjects[bb1]->getShape()->GetShapeType(),
				collisionObjects[bb2]->getShape()->GetShapeType());

			if(cf == NULL)
			{
				cf = CD->GetCollider(
				collisionObjects[bb2]->getShape()->GetShapeType(),
				collisionObjects[bb1]->getShape()->GetShapeType());
				is_reversed = true;
			}

			if	(is_reversed)
			{
				bb2 = p.bodyIndex1;
				bb1 = p.bodyIndex2;
			}

			assert(cf != NULL);

			if (int numContacts = cf(outCollisionPair[numCP], collisionObjects[bb1], collisionObjects[bb2]) )
			{
				outCollisionPair[numCP].body1 = collisionObjects[bb1];
				outCollisionPair[numCP].body2 = collisionObjects[bb2];
				outCollisionPair[numCP].numContacts = numContacts;
				for (int i = 0; i < CollisionPair::MAX_COLL_CONTACTS; i++)
				{
					outCollisionPair[numCP].contacts[i].accNormalImpulse = 0.0f;
					outCollisionPair[numCP].contacts[i].accTangentImpulse = 0.0f;
				}
				numCP++;
			}
		}

		for (int i= 0; i < worldGrid.size(); i++)
		{
			worldGrid[i].clear();
		}
};
//////////////////////////////////////////////////////////////////////////
void	QuadraticTimeBroadphaser::Init()
{}
//////////////////////////////////////////////////////////////////////////
void QuadraticTimeBroadphaser::DoBroadphase(CollisionAlgorithmDispatcher* CD, RigidBodyInterface** collisionObjects, int numCollisionObjects, CollisionPair* outCollisionPair, int& numCP)
{
	for (int i = 0; i < numCollisionObjects; ++i)
	{
		RigidBodyInterface* co1 = collisionObjects[i];
		for (int j = i + 1; j < numCollisionObjects; ++j)
		{
			RigidBodyInterface* co2 = collisionObjects[j];

			if (co1->getState() == STATIC && co2->getState() == STATIC)
			{
				continue;
			}

			if (co1->isDead() && co2->isDead())
			{
				continue;
			}

			Contact contacts[CollisionPair::MAX_COLL_CONTACTS];

			CollisionAlgorithm*	cf;

			bool is_reversed = false;
			int bb1 = co1->getID();
			int bb2 = co2->getID();

			cf = CD->GetCollider(
			collisionObjects[bb1]->getShape()->GetShapeType(),
			collisionObjects[bb2]->getShape()->GetShapeType());

			if(cf == NULL)
			{
				cf = CD->GetCollider(
				collisionObjects[bb2]->getShape()->GetShapeType(),
				collisionObjects[bb1]->getShape()->GetShapeType());
				is_reversed = true;
			}

			if	(is_reversed)
			{
				bb2 = co1->getID();
				bb1 = co2->getID();
			}

			if (int numContacts = cf(outCollisionPair[numCP], collisionObjects[bb1], collisionObjects[bb2]) )
			{
				outCollisionPair[numCP].body1 = collisionObjects[bb1];
				outCollisionPair[numCP].body2 = collisionObjects[bb2];
				outCollisionPair[numCP].numContacts = numContacts;
				for (int i = 0; i < CollisionPair::MAX_COLL_CONTACTS; i++)
				{
					outCollisionPair[numCP].contacts[i].accNormalImpulse = 0.0f;
					outCollisionPair[numCP].contacts[i].accTangentImpulse = 0.0f;
				}
				numCP++;
			}
		}
	}
};