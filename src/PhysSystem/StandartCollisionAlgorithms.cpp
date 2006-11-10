#include	"StandartCollisionAlgorithms.h"

#include	"math\closest_points.h"

int CollideCircleCircle(CollisionPair& cp, RigidBodyInterface* A, RigidBodyInterface* B)
{
	CircleShape* bodyA = (CircleShape*)A->getShape();
	CircleShape* bodyB = (CircleShape*)B->getShape();

	mt::vec2f normal = B->x - A->x;
	float d = normal.length();

	if (d > (bodyA->radius + bodyB->radius)) 
	{
		return 0;
	}

	normal *= 1.0f/d;

	cp.contacts[0].pdepth = d - bodyA->radius - bodyB->radius;
	cp.contacts[0].normal = normal;
	cp.contacts[0].pos = normal * bodyA->radius + A->x;
	return	1;
}

int CollideBoxCircle(CollisionPair& cp, RigidBodyInterface* A, RigidBodyInterface* B)
{
	BoxShape* bodyA = (BoxShape*)A->getShape();
	CircleShape* bodyB = (CircleShape*)B->getShape();

	mt::vec2f h = bodyA->width * 0.5f;

	float c = cosf(A->rot);
	float s = sinf(A->rot);
	
	mt::vec2f p[4] =
	{
		mt::vec2f(s * h.y - c * h.x + A->x.x, -s * h.x - c * h.y + A->x.y),
		mt::vec2f(c * h.x + s * h.y + A->x.x, s * h.x - c * h.y + A->x.y),
		mt::vec2f(c * h.x - s * h.y + A->x.x, s * h.x + c * h.y + A->x.y),
		mt::vec2f(-c * h.x - s * h.y + A->x.x, c * h.y - s * h.x + A->x.y)
	};

	int		closestLineIndex = -1;
	float	closestDistance = FLT_MAX;

	for (int i = 0; i < 4; i++) 
	{
		mt::vec2f	closestvec;
		closest_pt_point_seg(B->x,p[i],p[(i+1)%4],closestvec);
		float dis = (closestvec - B->x).sqrlength();
		if (dis < bodyB->radius * bodyB->radius) 
		{
			if (closestDistance > dis)
			{
				closestDistance = dis;
				closestLineIndex = i;
			}
		}
	}

	if (closestLineIndex > -1) 
	{
		cp.contacts[0].pdepth = sqrtf(closestDistance) - bodyB->radius;
		closest_pt_point_seg(B->x,p[closestLineIndex],p[(closestLineIndex+1)%4],cp.contacts[0].pos);
		norm_safe_v2(cp.contacts[0].normal,B->x - cp.contacts[0].pos);
		return 1;
	}
	else 
	{
		return 0;
	}
}

int CollideBoxBox(CollisionPair& cp, RigidBodyInterface* A, RigidBodyInterface* B)
{
	enum EdgeNumbers
	{
		NO_EDGE = 0,
		EDGE1,
		EDGE2,
		EDGE3,
		EDGE4
	};

	BoxShape* bodyA = (BoxShape*)A->getShape();
	BoxShape* bodyB = (BoxShape*)B->getShape();

	mt::vec2f hA = bodyA->width * 0.5f;
	mt::vec2f hB = bodyB->width * 0.5f;

	mt::vec2f posA = A->x;
	mt::vec2f posB = B->x;

	float c = cosf(A->rot);
	float s = sinf(A->rot);

	mt::vec2f dp = posB - posA;


	mt::vec2f dA(c * dp.x + s * dp.y,-s * dp.x + c * dp.y);

	float c2 = cosf(B->rot);
	float s2 = sinf(B->rot);

	mt::vec2f a1(c,s);
	mt::vec2f a2(-s,c);

	mt::vec2f b1(c2,s2);
	mt::vec2f b2(-s2,c2);

	mt::vec2f dB(c2 * dp.x + s2 * dp.y,-s2 * dp.x + c2 * dp.y);
	

	float v0x = abs(c * c2 + s * s2); 
	float v0y = abs(c * s2 - s * c2);
	float v1x = abs(s * c2 - c * s2);
	float v1y = abs(s * s2 + c * c2);

	mt::vec2f hBc;

	hBc.x = v0x * hB.x + v1x * hB.y;
	hBc.y = v0y * hB.x + v1y * hB.y;

	mt::vec2f faceA = mt::vec2f(abs(dA.x),abs(dA.y)) - hA - hBc;

	if (faceA.x > 0.0f || faceA.y > 0.0f)
	{
		return 0;
	}

	mt::vec2f hAc;

	hAc.x = v0x * hA.x + v0y * hA.y;
	hAc.y = v1x * hA.x + v1y * hA.y;

	mt::vec2f faceB = mt::vec2f(abs(dB.x),abs(dB.y)) - hAc - hB;


	if (faceB.x > 0.0f || faceB.y > 0.0f)
	{
		return 0;
	}

	enum Axis
	{
		FACE_A_X,
		FACE_A_Y,
		FACE_B_X,
		FACE_B_Y
	} axis;
	float pdepth;
	mt::vec2f normal;

	axis = FACE_A_X;
	pdepth = faceA.x;
	normal = dA.x > 0.0f ? a1 : -a1;

	if (faceA.y > 1.05f * pdepth + 0.01f * hA.y)
	{
		axis = FACE_A_Y;
		pdepth = faceA.y;
		normal = dA.y > 0.0f ? a2 : -a2;
	}

	if (faceB.x > 1.05f * pdepth + 0.01f * hB.x)
	{
		axis = FACE_B_X;
		pdepth = faceB.x;
		normal = dB.x > 0.0f ? b1 : -b1;
	}

	if (faceB.y > 1.05f * pdepth + 0.01f * hB.y)
	{
		axis = FACE_B_Y;
		pdepth = faceB.y;
		normal = dB.y > 0.0f ? b2 : -b2;
	}

	mt::vec2f frontNormal, sideNormal;
	mt::vec2f incidentEdge[2];
	float front, negSide, posSide;
	char negEdge, posEdge;

	switch (axis)
	{
		case FACE_A_X:
		{
			frontNormal = normal;
			front = dot_v2_v2(posA, frontNormal) + hA.x;
			sideNormal = a2;
			float side = dot_v2_v2(posA, sideNormal);
			negSide = -side + hA.y;
			posSide =  side + hA.y;
			negEdge = EDGE3;
			posEdge = EDGE1;
			compute_incident_edge_v2(incidentEdge, hB, posB, B->rot, frontNormal);
		}
		break;

		case FACE_A_Y:
		{
			frontNormal = normal;
			front = dot_v2_v2(posA, frontNormal) + hA.y;
			sideNormal = a1;
			float side = dot_v2_v2(posA, sideNormal);
			negSide = -side + hA.x;
			posSide =  side + hA.x;
			negEdge = EDGE2;
			posEdge = EDGE4;
			compute_incident_edge_v2(incidentEdge, hB, posB, B->rot, frontNormal);
		}
		break;

		case FACE_B_X:
		{
			frontNormal = -normal;
			front = dot_v2_v2(posB, frontNormal) + hB.x;
			sideNormal = b2;
			float side = dot_v2_v2(posB, sideNormal);
			negSide = -side + hB.y;
			posSide =  side + hB.y;
			negEdge = EDGE3;
			posEdge = EDGE1;
			compute_incident_edge_v2(incidentEdge, hA, posA, A->rot, frontNormal);
		}
		break;

		case FACE_B_Y:
		{
			frontNormal = -normal;
			front = dot_v2_v2(posB, frontNormal) + hB.y;
			sideNormal = b1;
			float side = dot_v2_v2(posB, sideNormal);
			negSide = -side + hB.x;
			posSide =  side + hB.x;
			negEdge = EDGE2;
			posEdge = EDGE4;
			compute_incident_edge_v2(incidentEdge, hA, posA, A->rot, frontNormal);
		}
		break;
	}

	mt::vec2f	clipPoints1[2];
	mt::vec2f	clipPoints2[2];

	int np = clip_segment_to_line_v2(clipPoints1, incidentEdge, -sideNormal, negSide, negEdge);

	if (np < 2)
	{
		return 0;
	}

	np = clip_segment_to_line_v2(clipPoints2, clipPoints1,  sideNormal, posSide, posEdge);

	if (np < 2)
	{
		return 0;
	}

	int numContacts = 0;

	for (int i = 0; i < 2; ++i)
	{
		float pdepth = dot_v2_v2(frontNormal, clipPoints2[i]) - front;
		if (pdepth <= 0)
		{
			cp.contacts[numContacts].pdepth = pdepth;
			cp.contacts[numContacts].normal = normal;
			cp.contacts[numContacts].pos = clipPoints2[i] - frontNormal * pdepth;
			++numContacts;
		}
	}

	return numContacts;
}