#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{
	MATH_FUNCTION_INLINE bool line_intersect_v2(
			const mt::vec2f & _begin,
			const mt::vec2f & _end,
			const mt::vec2f & _other_line_begin,
			const mt::vec2f & _other_line_end,
			mt::vec2f & intersection);
};

#	if MATH_FORCE_INLINE == 1
#	include "line_inline.h"
#	endif
