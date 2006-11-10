#include "CollisionShapes.h"
//////////////////////////////////////////////////////////////////////////
BoxShape::BoxShape(mt::vec2f	_width) 
	: width(_width), margin(0.5f)
{
	assert((width.x > 0) && (width.y > 0));
	evaluteBBox(0);
};
//////////////////////////////////////////////////////////////////////////
void BoxShape::evaluteBBox(float _rot)
{
	float c = fabsf(cos(_rot));
	float s = fabsf(sin(_rot));
	BoundingBox.x = (c * width.x + s * width.y) + margin;
	BoundingBox.y = (s * width.x + c * width.y) + margin;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f& BoxShape::getBBoxInfo() const
{
	return	BoundingBox;
}
//////////////////////////////////////////////////////////////////////////
int	BoxShape::getShapeType() const
{
	return	BOX;
}
//////////////////////////////////////////////////////////////////////////
CircleShape::CircleShape(float	_r) 
: radius(_r), margin(0.25f)
{
	assert(_r != 0);
	evaluteBBox(0);
};
//////////////////////////////////////////////////////////////////////////
void CircleShape::evaluteBBox(float _rot)
{
	BoundingBox.x = radius*2.0f + margin;
	BoundingBox.y = radius*2.0f + margin;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f& CircleShape::getBBoxInfo() const
{
	return BoundingBox;
}
//////////////////////////////////////////////////////////////////////////
int	CircleShape::getShapeType() const
{
	return	CIRCLE;
}