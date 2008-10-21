#	pragma once

namespace mt
{
	const float m_pi = 3.14159265358979323846f;
	const float m_half_pi = 1.57079632679489661923f;
	const float m_two_pi = 2.0f * m_pi;
	const float m_e = 2.71828182845904523536f;
	const float	m_deg2rad = m_pi / 180.0f;
	const float	m_rad2deg = 180.0f / m_pi;

}

#	define MATH_FORCE_INLINE 0

#	if MATH_FORCE_INLINE == 1
#	define MATH_INLINE inline
#	else
#	define MATH_INLINE
#	endif