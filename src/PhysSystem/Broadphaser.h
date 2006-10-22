#pragma once
#include "CommonCollision.h"
#include "CollisionAlgoDispatcher.h"
//////////////////////////////////////////////////////////////////////////
#include <stack>
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Broadphaser
{
public:
	virtual ~Broadphaser(){};
	virtual void Init() = 0;
	virtual void DoBroadphase(CollisionAlgorithmDispatcher* CD, RigidBodyInterface** collisionObjects, int numCollisionObjects, CollisionPair* outCollisionPair, int& numCP) = 0;
};
//////////////////////////////////////////////////////////////////////////
/* standard broadphasers: */
//////////////////////////////////////////////////////////////////////////
class LinearTimeBroadphaser : public Broadphaser
{
public:
	LinearTimeBroadphaser(int _ratio, int _gridSize);
	void	Init();
	void	DoBroadphase(CollisionAlgorithmDispatcher* CD, RigidBodyInterface** collisionObjects, int numCollisionObjects, CollisionPair* outCollisionPair, int& numCP);
private:
	struct CollisionObjectsPair
	{
		CollisionObjectsPair(int i, int j) : bodyIndex1(i),bodyIndex2(j){};
		int bodyIndex1;
		int bodyIndex2;
	};
	enum {MAX_BODIES = 1000};
	bool collisions[MAX_BODIES][MAX_BODIES];
	std::stack<CollisionObjectsPair> collisionStack;
	std::vector<std::vector<RigidBodyInterface*>> worldGrid;
	int	gridSize;
	int ratio;
};
//////////////////////////////////////////////////////////////////////////
class QuadraticTimeBroadphaser : public Broadphaser
{
public:
	void	Init();
	void	DoBroadphase(CollisionAlgorithmDispatcher* CD, RigidBodyInterface** collisionObjects, int numCollisionObjects, CollisionPair* outCollisionPair, int& numCP);
};
//////////////////////////////////////////////////////////////////////////