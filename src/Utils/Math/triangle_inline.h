#	include "triangle.h"

#	include "line2.h"

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
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool triangle_intersect_triangle( const mt::vec2f & a1, const mt::vec2f & a2, const mt::vec2f & a3, const mt::vec2f & b1, const mt::vec2f & b2, const mt::vec2f & b3 )
	{
		mt::line2f la1;
		mt::line2f la2;
		mt::line2f la3;
		mt::line_from_two_point_v2( la1, a2, a1 );
		mt::line_from_two_point_v2( la2, a3, a2 );
		mt::line_from_two_point_v2( la3, a1, a3 );

		mt::line2f lb1;
		mt::line2f lb2;
		mt::line2f lb3;
		mt::line_from_two_point_v2( lb1, b2, b1 );
		mt::line_from_two_point_v2( lb2, b3, b2 );
		mt::line_from_two_point_v2( lb3, b1, b3 );

		if( mt::line_dot_point_v2( la1, b1 ) > 0.f &&
			mt::line_dot_point_v2( la1, b2 ) > 0.f &&
			mt::line_dot_point_v2( la1, b3 ) > 0.f )
		{
			return false;
		}

		if( mt::line_dot_point_v2( la2, b1 ) > 0.f &&
			mt::line_dot_point_v2( la2, b2 ) > 0.f &&
			mt::line_dot_point_v2( la2, b3 ) > 0.f )
		{
			return false;
		}

		if( mt::line_dot_point_v2( la3, b1 ) > 0.f &&
			mt::line_dot_point_v2( la3, b2 ) > 0.f &&
			mt::line_dot_point_v2( la3, b3 ) > 0.f )
		{
			return false;
		}

		if( mt::line_dot_point_v2( lb1, a1 ) > 0.f &&
			mt::line_dot_point_v2( lb1, a2 ) > 0.f &&
			mt::line_dot_point_v2( lb1, a3 ) > 0.f )
		{
			return false;
		}

		if( mt::line_dot_point_v2( lb2, a1 ) > 0.f &&
			mt::line_dot_point_v2( lb2, a2 ) > 0.f &&
			mt::line_dot_point_v2( lb2, a3 ) > 0.f )
		{
			return false;
		}

		if( mt::line_dot_point_v2( lb3, a1 ) > 0.f &&
			mt::line_dot_point_v2( lb3, a2 ) > 0.f &&
			mt::line_dot_point_v2( lb3, a3 ) > 0.f )
		{
			return false;
		}

		return true;
	}
}