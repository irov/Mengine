#	pragma once

#	include <stdint.h>

namespace mt
{
	const float m_pi = 3.14159265358979323846f;
	const float m_inv_pi = 1.f / m_pi;
	const float m_negative_pi = -m_pi;
	const float m_half_pi = m_pi * 0.5f;
	const float m_two_pi = 2.f * m_pi;
	const float m_inv_two_pi = 1.f / m_two_pi;
	const float m_e = 2.71828182845904523536f;
	const float	m_deg2rad = m_pi / 180.f;
	const float	m_rad2deg = 180.f / m_pi;
	const float m_eps = 0.00001f;
	const float m_neps = -m_eps;
}

#	ifndef _DEBUG
#	define MATH_FORCE_INLINE 1
#	endif

#	define MATH_INLINE inline

#	if MATH_FORCE_INLINE == 1
#	define MATH_FUNCTION_INLINE MATH_INLINE static
#	define MATH_METHOD_INLINE MATH_INLINE
#	else
#	define MATH_FUNCTION_INLINE
#	define MATH_METHOD_INLINE
#	endif