#pragma once 
#include "interfaces.h"
#include <float.h>
#include "math\vec2.h"
//////////////////////////////////////////////////////////////////////////
enum	SHAPETYPE
{
	CIRCLE = 0,
	BOX = 1,
};
//////////////////////////////////////////////////////////////////////////
class	BoxShape
	: public CollisionShape
{
public:
	BoxShape(mt::vec2f	_width);
	bool IsConvex() const;
	void EvaluteBBox(float _rot);
	const mt::vec2f& GetBBoxInfo() const;
	int	GetShapeType() const;
	void SetMargin(float eps);
	float GetMargin() const;
public:
	mt::vec2f width;
private:
	mt::vec2f BoundingBox;
	float margin;
};
//////////////////////////////////////////////////////////////////////////
class	CircleShape 
	: public CollisionShape
{
public:
	CircleShape::CircleShape(float	_r);
	bool IsConvex() const;
	void EvaluteBBox(float _rot);
	const mt::vec2f& GetBBoxInfo() const;
	int	GetShapeType() const;
	void SetMargin(float eps);
	float GetMargin() const;
public:
	float radius;
private:
	mt::vec2f BoundingBox;
	float margin;
};