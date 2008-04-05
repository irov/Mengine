#	include "vec4.h"

namespace mt
{
	const vec4f vec4f::zero_v4(0, 0, 0, 0);
}

#	if MATH_FORCE_INLINE == 0
#	include "vec4_inline.h"
#	endif