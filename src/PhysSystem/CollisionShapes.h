#pragma once 
#include "interfaces.h"
#include <float.h>
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
	void evaluteBBox(float _rot);
	const mt::vec2f& getBBoxInfo() const;
	int	getShapeType() const;
	void setMargin(float eps);
	float getMargin() const;
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
	void evaluteBBox(float _rot);
	const mt::vec2f& getBBoxInfo() const;
	int	getShapeType() const;
	void setMargin(float eps);
	float getMargin() const;
public:
	float radius;
private:
	mt::vec2f BoundingBox;
	float margin;
};