#include "bv.h"

void calculate_aabb_from_obb(mt::vec2f& min0, mt::vec2f& max0, const mt::vec2f& _size, const mt::vec2f& _offset, const mt::mat3f& _wm)
{
	mt::vec2f bounds[4];

	bounds[0].x = _offset.x;
	bounds[0].y = _offset.y;

	bounds[1].x = _offset.x + _size.x;
	bounds[1].y = _offset.y;

	bounds[2].x = _offset.x;
	bounds[2].y = _offset.y + _size.y;

	bounds[3].x = _offset.x + _size.x;
	bounds[3].y = _offset.y + _size.y;

	mt::mul_v2_m3(min0, bounds[0], _wm );

	max0 = min0;

	for(size_t i = 1; i < 4; ++i)
	{
		mt::vec2f temp;
		mt::mul_v2_m3(temp, bounds[i], _wm );
		if (max0.x < temp.x) max0.x = temp.x;
		if (max0.y < temp.y) max0.y = temp.y;
		if (min0.x > temp.x) min0.x = temp.x;
		if (min0.y > temp.y) min0.y = temp.y;
	}
}