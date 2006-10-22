#pragma once
#include "CollisionShapes.h"
#include "CommonCollision.h"

int CollideBoxBox(CollisionPair& cp, RigidBodyInterface* A, RigidBodyInterface* B);
int CollideCircleCircle(CollisionPair& cp, RigidBodyInterface* A, RigidBodyInterface* B);
int CollideBoxCircle(CollisionPair& cp, RigidBodyInterface* A, RigidBodyInterface* B);
