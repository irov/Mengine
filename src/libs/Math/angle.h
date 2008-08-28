#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{
	MATH_INLINE float angle_norm360( float _angle );
	MATH_INLINE float angle_norm180( float _angle );
	MATH_INLINE float angle_delta_deg( float _angle1, float _angle2 );
	MATH_INLINE float acos32( float _x );
	MATH_INLINE float signed_angle(const mt::vec2f & _vec);
	MATH_INLINE float angle_in_interval_deg( float _angle, float _min, float _max );
}

#	if MATH_FORCE_INLINE == 1
#	include "angle_inline.h"
#	endif