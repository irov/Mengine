#	include "triangle.h"

namespace mt
{	
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool triangle_intersect_v2( const mt::vec2f & _a, const mt::vec2f & _b, const mt::vec2f & _c, const mt::vec2f & _p )
	{
		mt::vec2f v0 = _c - _a;
		mt::vec2f v1 = _b - _a;
		mt::vec2f v2 = _p - _a;

		float dot00 = mt::dot_v2_v2(v0, v0);
		float dot01 = mt::dot_v2_v2(v0, v1);
		float dot02 = mt::dot_v2_v2(v0, v2);
		float dot11 = mt::dot_v2_v2(v1, v1);
		float dot12 = mt::dot_v2_v2(v1, v2);

		float invDenom = 1.f / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return ((u > 0.f) && (v > 0.f) && (u + v < 1.f));
	}
}