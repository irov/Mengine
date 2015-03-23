#	include "line2.h"

#	include "utils.h"

namespace mt
{	
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void line_from_two_point_v2( mt::line & _line, const mt::vec2f & _a, const mt::vec2f & _b )
	{
		_line.a = _a.y - _b.y;
		_line.b = _b.x - _a.x;
		_line.c = _a.x * _b.y - _b.x * _a.y;
	}
	//////////////////////////////////////////////////////////////////////////
	static MENGINE_MATH_INLINE float s_line_dot( float _a, float _b, float _c, float _d )
	{
		return _a * _d - _c * _b;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool line_intersect_v2( const mt::line & _l1, const mt::line & _l2, mt::vec2f & _p )
	{
		float zn = s_line_dot( _l1.a, _l1.b, _l2.a, _l2.b );

		if( mt::cmp_f_z( zn ) == true )
		{
			return false;
		}

		_p.x = - s_line_dot( _l1.c, _l1.b, _l2.c, _l2.b ) / zn;
		_p.y = - s_line_dot( _l1.a, _l1.c, _l2.a, _l2.c ) / zn;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool line_segment_intersect_v2(
		const mt::vec2f & _a0, 
		const mt::vec2f & _a1,
		const mt::vec2f & _b0, 
		const mt::vec2f & _b1, 
		mt::vec2f & _intersectionPoint ) 
	{
		if( _a0 == _b0 || _a0 == _b1 || _a1 == _b0 || _a1 == _b1 ) 
		{
			return false;
		}

		float x1 = _a0.x; 
		float y1 = _a0.y;
		float x2 = _a1.x; 
		float y2 = _a1.y;
		float x3 = _b0.x; 
		float y3 = _b0.y;
		float x4 = _b1.x; 
		float y4 = _b1.y;
		
		if( max_f(x1,x2) < max_f(x3,x4) || max_f(x3,x4) < max_f(x1,x2) )
		{
			return false;
		}

		if( max_f(y1,y2) < max_f(y3,y4) || max_f(y3,y4) < max_f(y1,y2) ) 
		{
			return false;
		}
		
		float ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3));
		float ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3));
		float denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
		
		if( denom == 0.f )
		{
			return false;
		}

		float denom_inv = 1.f / denom;

		ua *= denom_inv;
		ub *= denom_inv;
		
		if( (0.f > ua) || (ua > 1.f) || (0.f > ub) || (ub > 1.f) ) 
		{
			return false;
		}
		
		_intersectionPoint.x = (x1 + ua * (x2 - x1));
		_intersectionPoint.y = (y1 + ua * (y2 - y1));

		return true;		
	}
}
