#	pragma once

#	include "config.h"

#	include "segment2.h"

namespace mt
{
	class capsule2
	{
	public:
		capsule2()
		{
		}

		capsule2( const mt::segment2 & _segment, float _radius )
			: segment(_segment)
			, radius(_radius)
		{
		}

	public:
		mt::segment2 segment;
		float radius;
	};

	MENGINE_MATH_FUNCTION_INLINE bool capsule2_intersect_segment(
		const mt::capsule2 & _capsule,
		const mt::segment2 & _segment );

	MENGINE_MATH_FUNCTION_INLINE bool capsule2_intersect_capsule2(
		const mt::capsule2 & _capsule0,
		const mt::capsule2 & _capsule1 );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#		include "capsule2_inline.h"
#	endif
