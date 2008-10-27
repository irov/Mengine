#	include "vec2.h"

namespace	mt
{
	const vec2f vec2f::zero_v2(0, 0);
}

#	if MATH_FORCE_INLINE == 0
#	include "vec2_inline.h"
#	endif


