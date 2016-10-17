#	include "line2.h"

#	include "utils.h"

namespace mt
{	
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void line_from_two_point_v2( mt::line2f & _line, const mt::vec2f & _a, const mt::vec2f & _b )
	{
		_line.a = _a.y - _b.y;
		_line.b = _b.x - _a.x;
		_line.c = _a.x * _b.y - _b.x * _a.y;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE float line_dot_point_v2( mt::line2f & _line, const mt::vec2f & _a )
	{
		return _line.a * _a.x + _line.b * _a.y + _line.c;
	}
	//////////////////////////////////////////////////////////////////////////
	static MENGINE_MATH_INLINE float s_line_dot( float _a, float _b, float _c, float _d )
	{
		return _a * _d - _c * _b;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool line_intersect_v2( const mt::line2f & _l1, const mt::line2f & _l2, mt::vec2f & _p )
	{
		float zn = s_line_dot( _l1.a, _l1.b, _l2.a, _l2.b );

		if( mt::equal_f_z( zn ) == true )
		{
			return false;
		}

		_p.x = - s_line_dot( _l1.c, _l1.b, _l2.c, _l2.b ) / zn;
		_p.y = - s_line_dot( _l1.a, _l1.c, _l2.a, _l2.c ) / zn;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void line_intersect_v2_np( const mt::line2f & _l1, const mt::line2f & _l2, mt::vec2f & _p )
	{
		float zn = s_line_dot( _l1.a, _l1.b, _l2.a, _l2.b );

		_p.x = -s_line_dot( _l1.c, _l1.b, _l2.c, _l2.b ) / zn;
		_p.y = -s_line_dot( _l1.a, _l1.c, _l2.a, _l2.c ) / zn;
	}
}
