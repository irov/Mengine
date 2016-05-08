#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{	
	class segment2
	{
	public:
		segment2()
		{
		}

		segment2( const mt::vec2f & _a, const mt::vec2f & _b )
			: a( _a )
			, b( _b )
		{
		}

		segment2( const segment2 & _segment )
			: a(_segment.a)
			, b(_segment.b)
		{
		}

	public:
		mt::vec2f a;
		mt::vec2f b;
	};

	MENGINE_MATH_FUNCTION_INLINE float segment2_distance_squared(
		const mt::segment2 & _segment0,
		const mt::segment2 & _segment1,
		float & _sc,
		float & _tc );

	MENGINE_MATH_FUNCTION_INLINE bool segment2_intersect_segment2(
		const mt::segment2 & _segment0,
		const mt::segment2 & _segment1,
		mt::vec2f & _intersectionPoint );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#		include "segment2_inline.h"
#	endif
