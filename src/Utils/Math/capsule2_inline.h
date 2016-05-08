#	include "capsule2.h"

#	include <math.h>

#	include "utils.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool capsule2_intersect_segment(
		const mt::capsule2 & _capsule,
		const mt::segment2 & _segment )
	{
		float s_c, t_c;
		
		return mt::segment2_distance_squared( _capsule.segment, _segment, s_c, t_c ) <= _capsule.radius * _capsule.radius;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool capsule2_intersect_capsule2(
		const mt::capsule2 & _capsule0,
		const mt::capsule2 & _capsule1,
		mt::vec2f & _point,
		mt::vec2f & _normal,
		float & _penetration )
	{
		float radiusSum = _capsule0.radius + _capsule1.radius;

		// if colliding
		float s, t;
		float distancesq = mt::segment2_distance_squared( _capsule0.segment, _capsule1.segment, s, t );

		if( distancesq <= radiusSum * radiusSum )
		{
			// calculate our values
			mt::vec2f nearPoint0 = (1.0f - s) * _capsule0.segment.a + s * _capsule0.segment.b;
			mt::vec2f nearPoint1 = (1.0f - t) * _capsule1.segment.a + t * _capsule1.segment.b;

			mt::dir_v2_v2( _normal, nearPoint1, nearPoint0 );

			// penetration is distance - radii
			float distance = ::sqrtf( distancesq );
			
			_penetration = radiusSum - distance;
			
			_point = 0.5f * (nearPoint0 + _capsule0.radius * _normal) + 0.5f * (nearPoint1 - _capsule1.radius * _normal);

			return true;
		}

		return false;
	}
}