#include "bv.h"

void calculate_aabb_from_obb(mt::vec3f& min0, mt::vec3f& max0, const mt::vec2f& _pos, const mt::vec2f& _size,  const mt::mat3f& _wm)
{
	mt::vec2f bounds[4];

	bounds[0].x = _pos.x;
	bounds[0].y = _pos.y;

	bounds[1].x = _pos.x + _size.x;
	bounds[1].y = _pos.y;

	bounds[2].x = _pos.x;
	bounds[2].y = _pos.y + _size.y;

	bounds[3].x = _pos.x + _size.x;
	bounds[3].y = _pos.y + _size.y;

	mt::mul_v3_m3(min0, mt::vec3f(bounds[0],1), _wm );

	max0 = min0;

	for(size_t i = 1; i < 4; ++i)
	{
		mt::vec3f temp;
		mt::mul_v3_m3(temp, mt::vec3f(bounds[i],1), _wm );
		if (max0.x < temp.x) max0.x = temp.x;
		if (max0.y < temp.y) max0.y = temp.y;
		if (min0.x > temp.x) min0.x = temp.x;
		if (min0.y > temp.y) min0.y = temp.y;
	}
}