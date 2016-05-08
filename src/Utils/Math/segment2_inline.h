#	include "segment2.h"

#	include "utils.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE float segment2_distance_squared(
		const mt::segment2 & _segment0,
		const mt::segment2 & _segment1,
		float & _sc,
		float & _tc )
	{
		// compute intermediate parameters
		mt::vec2f w0 = _segment0.a - _segment1.a;

		mt::vec2f a_direction = _segment0.b - _segment0.a;
		mt::vec2f b_direction = _segment1.b - _segment1.a;

		float a = mt::dot_v2_v2( a_direction, a_direction );
		float b = mt::dot_v2_v2( a_direction, b_direction );
		float c = mt::dot_v2_v2( b_direction, b_direction );
		float d = mt::dot_v2_v2( a_direction, w0 );
		float e = mt::dot_v2_v2( b_direction, w0 );

		float denom = a*c - b*b;
		// parameters to compute s_c, t_c
		float sn, sd, tn, td;

		// if denom is zero, try finding closest point on segment1 to origin0
		if( mt::equal_f_z( denom ) == true )
		{
			// clamp s_c to 0
			sd = td = c;
			sn = 0.f;
			tn = e;
		}
		else
		{
			// clamp s_c within [0,1]
			sd = td = denom;
			sn = b*e - c*d;
			tn = a*e - b*d;

			// clamp s_c to 0
			if( sn < 0.f )
			{
				sn = 0.f;
				tn = e;
				td = c;
			}
			// clamp s_c to 1
			else if( sn > sd )
			{
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		// clamp t_c within [0,1]
		// clamp t_c to 0
		if( tn < 0.f )
		{
			_tc = 0.f;
			// clamp s_c to 0
			if( -d < 0.f )
			{
				_sc = 0.f;
			}
			// clamp s_c to 1
			else if( -d > a )
			{
				_sc = 1.f;
			}
			else
			{
				_sc = -d / a;
			}
		}
		// clamp t_c to 1
		else if( tn > td )
		{
			_tc = 1.f;
			// clamp s_c to 0
			if( (-d + b) < 0.f )
			{
				_sc = 0.f;
			}
			// clamp s_c to 1
			else if( (-d + b) > a )
			{
				_sc = 1.f;
			}
			else
			{
				_sc = (-d + b) / a;
			}
		}
		else
		{
			_tc = tn / td;
			_sc = sn / sd;
		}

		// compute difference vector and distance squared
		mt::vec2f wc = w0 + _sc * a_direction - _tc * b_direction;

		float wcd = mt::dot_v2_v2( wc, wc );

		return wcd;
	}   // End of ::DistanceSquared()
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool segment2_intersect_segment2(
		const mt::segment2 & _segment0,
		const mt::segment2 & _segment1,
		mt::vec2f & _intersectionPoint )
	{
		if( _segment0.a == _segment1.a || _segment0.a == _segment1.b || _segment0.b == _segment1.a || _segment0.b == _segment1.b )
		{
			return false;
		}

		float x1 = _segment0.a.x;
		float y1 = _segment0.a.y;
		float x2 = _segment0.b.x;
		float y2 = _segment0.b.y;
		float x3 = _segment1.a.x;
		float y3 = _segment1.a.y;
		float x4 = _segment1.b.x;
		float y4 = _segment1.b.y;

		if( max_f( x1, x2 ) < max_f( x3, x4 ) || max_f( x3, x4 ) < max_f( x1, x2 ) )
		{
			return false;
		}

		if( max_f( y1, y2 ) < max_f( y3, y4 ) || max_f( y3, y4 ) < max_f( y1, y2 ) )
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