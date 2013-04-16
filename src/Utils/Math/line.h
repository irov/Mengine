#	pragma once

#	include "config.h"

#	include "vec2.h"

#	include <vector>
#	include <algorithm>

namespace mt
{
	MATH_FUNCTION_INLINE bool line_intersect_v2(
			const mt::vec2f & begin_,
			const mt::vec2f & end_,
			const mt::vec2f & other_line_begin_,
			const mt::vec2f & other_line_end_,
			mt::vec2f & intersection);
};

#	if MATH_FORCE_INLINE == 1
#	include "line_inline.h"
#	endif
