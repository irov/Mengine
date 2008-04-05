#	include "vec3.h"

namespace	mt
{
	const vec3f vec3f::zero_v3(0, 0, 0);
}

#	if MATH_FORCE_INLINE == 0
#	include "vec3_inline.h"
#	endif