#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{
	struct line
	{
		float a;
		float b;
		float c;
	};

	MENGINE_MATH_FUNCTION_INLINE void line_from_two_point_v2( mt::line & _line, const mt::vec2f & _a, const mt::vec2f & _b );
	MENGINE_MATH_FUNCTION_INLINE bool line_intersect_v2( const mt::line & _l1, const mt::line & _l2, mt::vec2f & _p );
	MENGINE_MATH_FUNCTION_INLINE void line_intersect_v2_np( const mt::line & _l1, const mt::line & _l2, mt::vec2f & _p );
};

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "line2_inline.h"
#	endif
