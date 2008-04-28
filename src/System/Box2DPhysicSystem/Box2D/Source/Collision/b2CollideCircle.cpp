/*
* Copyright (c) 2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "b2Collision.h"
#include "Shapes/b2CircleShape.h"
#include "Shapes/b2PolygonShape.h"

void b2CollideCircles(
	b2Manifold* manifold,
	const b2CircleShape* circle1, const b2XForm& xf1,
	const b2CircleShape* circle2, const b2XForm& xf2)
{
	manifold->pointCount = 0;

	b2Vec2 p1 = b2Mul(xf1, circle1->m_localPosition);
	b2Vec2 p2 = b2Mul(xf2, circle2->m_localPosition);

	b2Vec2 d = p2 - p1;
	float32 distSqr = b2Dot(d, d);
	float32 radiusSum = circle1->m_radius + circle2->m_radius;
	if (distSqr > radiusSum * radiusSum)
	{
		return;
	}

	float32 separation;
	if (distSqr < FLOAT32_EPSILON)
	{
		separation = -radiusSum;
		manifold->normal.Set(0.0f, 1.0f);
	}
	else
	{
		float32 dist = b2Sqrt(distSqr);
		separation = dist - radiusSum;
		float32 a = 1.0f / dist;
		manifold->normal.x = a * d.x;
		manifold->normal.y = a * d.y;
	}

	manifold->pointCount = 1;
	manifold->points[0].id.key = 0;
	manifold->points[0].separation = separation;

	p1 += circle1->m_radius * manifold->normal;
	p2 -= circle2->m_radius * manifold->normal;

	b2Vec2 p = 0.5f * (p1 + p2);

	manifold->points[0].localPoint1 = b2MulT(xf1, p);
	manifold->points[0].localPoint2 = b2MulT(xf2, p);
}

void b2CollidePolygonAndCircle(
	b2Manifold* manifold,
	const b2PolygonShape* polygon, const b2XForm& xf1,
	const b2CircleShape* circle, const b2XForm& xf2)
{
	manifold->pointCount = 0;

	// Compute circle position in the frame of the polygon.
	b2Vec2 c = b2Mul(xf2, circle->m_localPosition);
	b2Vec2 cLocal = b2MulT(xf1, c);

	// Find the min separating edge.
	int32 normalIndex = 0;
	float32 separation = -FLOAT32_MAX;
	const float32 radius = circle->m_radius;
	for (int32 i = 0; i < polygon->m_vertexCount; ++i)
	{
		float32 s = b2Dot(polygon->m_normals[i], cLocal - polygon->m_vertices[i]);

		if (s > radius)
		{
			// Early out.
			return;
		}

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// If the center is inside the polygon ...
	if (separation < FLOAT32_EPSILON)
	{
		manifold->pointCount = 1;
		manifold->normal = b2Mul(xf1.R, polygon->m_normals[normalIndex]);
		manifold->points[0].id.features.incidentEdge = (uint8)normalIndex;
		manifold->points[0].id.features.incidentVertex = b2_nullFeature;
		manifold->points[0].id.features.referenceEdge = b2_nullFeature;
		manifold->points[0].id.features.flip = 0;
		b2Vec2 position = c - radius * manifold->normal;
		manifold->points[0].localPoint1 = b2MulT(xf1, position);
		manifold->points[0].localPoint2 = b2MulT(xf2, position);
		manifold->points[0].separation = separation - radius;
		return;
	}

	// Project the circle center onto the edge segment.
	int32 vertIndex1 = normalIndex;
	int32 vertIndex2 = vertIndex1 + 1 < polygon->m_vertexCount ? vertIndex1 + 1 : 0;
	b2Vec2 e = polygon->m_vertices[vertIndex2] - polygon->m_vertices[vertIndex1];

	float32 length = e.Normalize();

	// If the edge length is zero ...
	if (length < FLOAT32_EPSILON)
	{
		b2Vec2 d = cLocal - polygon->m_vertices[vertIndex1];
		float32 dist = d.Normalize();
		if (dist > radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->normal = b2Mul(xf1.R, d);
		manifold->points[0].id.features.incidentEdge = b2_nullFeature;
		manifold->points[0].id.features.incidentVertex = (uint8)vertIndex1;
		manifold->points[0].id.features.referenceEdge = b2_nullFeature;
		manifold->points[0].id.features.flip = 0;
		b2Vec2 position = c - radius * manifold->normal;
		manifold->points[0].localPoint1 = b2MulT(xf1, position);
		manifold->points[0].localPoint2 = b2MulT(xf2, position);
		manifold->points[0].separation = dist - radius;
		return;
	}

	// Project the center onto the edge.
	float32 u = b2Dot(cLocal - polygon->m_vertices[vertIndex1], e);
	manifold->points[0].id.features.incidentEdge = b2_nullFeature;
	manifold->points[0].id.features.incidentVertex = b2_nullFeature;
	manifold->points[0].id.features.referenceEdge = b2_nullFeature;
	manifold->points[0].id.features.flip = 0;
	b2Vec2 p;
	if (u <= 0.0f)
	{
		p = polygon->m_vertices[vertIndex1];
		manifold->points[0].id.features.incidentVertex = (uint8)vertIndex1;
	}
	else if (u >= length)
	{
		p = polygon->m_vertices[vertIndex2];
		manifold->points[0].id.features.incidentVertex = (uint8)vertIndex2;
	}
	else
	{
		p = polygon->m_vertices[vertIndex1] + u * e;
		manifold->points[0].id.features.incidentEdge = (uint8)vertIndex1;
	}

	b2Vec2 d = cLocal - p;
	float32 dist = d.Normalize();
	if (dist > radius)
	{
		return;
	}

	manifold->pointCount = 1;
	manifold->normal = b2Mul(xf1.R, d);
	b2Vec2 position = c - radius * manifold->normal;
	manifold->points[0].localPoint1 = b2MulT(xf1, position);
	manifold->points[0].localPoint2 = b2MulT(xf2, position);
	manifold->points[0].separation = dist - radius;
}
