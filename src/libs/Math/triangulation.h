#	pragma once

#	include "config.h"

#	include "mat3.h"
#	include "polygon.h"

#	include <vector>
#	include <algorithm>

namespace mt
{
	MATH_INLINE bool triangulate_polygon(const std::vector<mt::vec2f> & _polygon,
						  std::vector<mt::vec2f> & _result);

	MATH_INLINE bool triangulate_polygon(const mt::polygon & _polygon,
						  std::vector<mt::vec2f> & _result);
};

#	if MATH_FORCE_INLINE == 1
#	include "triangulation_inline.h"
#	endif