#	pragma once

#	include "config.h"

namespace mt
{
	MENGINE_MATH_FUNCTION_INLINE float angle_norm(float _angle);
	MENGINE_MATH_FUNCTION_INLINE float angle_norm2(float _angle);
	MENGINE_MATH_FUNCTION_INLINE void angle_correct_interpolate_from_to(float _from, float _to, float & _correct_from, float & _correct_to);
	MENGINE_MATH_FUNCTION_INLINE float angle_length( float _angle1, float _angle2 );
	MENGINE_MATH_FUNCTION_INLINE float angle_norm360( float _angle );
	MENGINE_MATH_FUNCTION_INLINE float angle_norm180( float _angle );	
	MENGINE_MATH_FUNCTION_INLINE float angle_delta_deg( float _angle1, float _angle2 );
	MENGINE_MATH_FUNCTION_INLINE float acos32( float _x );
	MENGINE_MATH_FUNCTION_INLINE float angle_in_interval_deg( float _angle, float _min, float _max );	
	MENGINE_MATH_FUNCTION_INLINE float cosf_fast( float x );
	MENGINE_MATH_FUNCTION_INLINE float sinf_fast( float x );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "angle_inline.h"
#	endif
