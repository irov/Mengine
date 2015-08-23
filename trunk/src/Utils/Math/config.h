#	pragma once

#	include <stddef.h>

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
	const float m_eps2 = m_eps * 10.f;	
	const float m_neps = -m_eps;
	const float m_1_eps = 1.f + m_eps;
	const float m_1_neps = 1.f - m_eps;
}

#	ifndef MENGINE_MATH_FORCE_INLINE
#		ifdef _DEBUG
#			define MENGINE_MATH_FORCE_INLINE 0
#		else
#			define MENGINE_MATH_FORCE_INLINE 1
#		endif
#	endif


#	define MENGINE_MATH_INLINE inline

#	if MENGINE_MATH_FORCE_INLINE == 1
#		define MENGINE_MATH_FUNCTION_INLINE MENGINE_MATH_INLINE static
#		define MENGINE_MATH_METHOD_INLINE MENGINE_MATH_INLINE
#	else
#		define MENGINE_MATH_FUNCTION_INLINE
#		define MENGINE_MATH_METHOD_INLINE
#	endif