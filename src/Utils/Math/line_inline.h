#	include "line.h"

namespace mt
{	
	inline float maxf( float _x, float _y )
	{
		return ( _x > _y ) ? _x : _y;
	}

	MATH_FUNCTION_INLINE bool line_intersect_v2(
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
		
		if( maxf(x1,x2) < maxf(x3,x4) || maxf(x3,x4) < maxf(x1,x2) )
		{
			return false;
		}

		if( maxf(y1,y2) < maxf(y3,y4) || maxf(y3,y4) < maxf(y1,y2) ) 
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
