#	pragma once

namespace mt
{
	const float m_pi = 3.14159265358979323846f;
	const float m_2_pi = 1.57079632679489661923f;
}

#	define MATH_FORCE_INLINE 1

#	if MATH_FORCE_INLINE == 1
#	define MATH_INLINE inline
#	else
#	define MATH_INLINE
#	endif
