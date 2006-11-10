#include "Joint.h"

ElasticJoint::ElasticJoint(RigidBodyInterface* b1, RigidBodyInterface* b2, const mt::vec2f& anchor)
{
	body1 = b1;
	body2 = b2;

	float c = cosf(body1->rot);
	float s = sinf(body1->rot);
	mt::vec2f v = anchor - body1->x;
	localAnchor1 = mt::vec2f(c * v.x + s * v.y, c * v.y - s * v.x );

	c = cosf(body1->rot); s = sinf(body1->rot);
	v = anchor - body2->x;
	localAnchor2 = mt::vec2f(c * v.x + s * v.y, c * v.y - s * v.x );

	accumulatedImpulse = mt::vec2f(0.0f, 0.0f);
	relaxation = 1.0f;
}

void	ElasticJoint::calculate(float inv_dt)
{
	body1->wake();
	body2->wake();

	float cos = cosf(body1->rot);
	float sin = sinf(body1->rot);

	r1.x = cos * localAnchor1.x - sin * localAnchor1.y;
	r1.y = sin * localAnchor1.x + cos * localAnchor1.y;

	cos = cosf(body2->rot);
	sin = sinf(body2->rot);

	r2.x = cos * localAnchor2.x - sin * localAnchor2.y;
	r2.y = sin * localAnchor2.x + cos * localAnchor2.y;

	float I_1y = body1->I_1 * r1.y;
	float I_1y2 = body2->I_1 * r2.y;

	float a = body1->m_1 + body2->m_1 + I_1y * r1.y + I_1y2 * r2.y;
	float b = -I_1y * r1.x - I_1y2 * r2.x;
	float c = -I_1y * r1.x - I_1y2 * r2.x;	
	float d = body1->m_1 + body1->I_1 * r1.x * r1.x + body2->m_1 + body2->I_1 * r2.x * r2.x;
	float det = a * d - b * c;

	det = 1.0f / det;

	M[0] = det * d;
	M[1] = -det * b;
	M[2] = -det * c;
	M[3] = det * a;

	bias = (body2->x + r2 - body1->x - r1) * -0.1f * inv_dt;
	accumulatedImpulse *= relaxation;

	body1->v -= accumulatedImpulse * body1->m_1;
	body1->w -= mt::pseudo_cross_v2(r1, accumulatedImpulse) * body1->I_1;

	body2->v += accumulatedImpulse * body2->m_1;
	body2->w += mt::pseudo_cross_v2(r2, accumulatedImpulse) * body2->I_1;
}

void	ElasticJoint::integrate()
{
	body1->wake();
	body2->wake();

	mt::vec2f dv = body2->v + mt::perp(r2) * body2->w- body1->v - mt::perp(r1) * body1->w;

	mt::vec2f v = (bias-dv);

	mt::vec2f impulse(M[0] * v.x + M[1] * v.y, M[2] * v.x + M[3] * v.y);

	body1->v -= impulse * body1->m_1;
	body1->w -= mt::pseudo_cross_v2(r1, impulse) * body1->I_1;

	body2->v += impulse * body2->m_1;
	body2->w += mt::pseudo_cross_v2(r2, impulse) * body2->I_1;

	accumulatedImpulse += impulse;
}