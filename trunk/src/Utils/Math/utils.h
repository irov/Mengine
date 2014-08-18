#	pragma once

#	include "config.h"

#	include <math.h>

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	MATH_INLINE T crop( T _value, T _min, T _max )
	{
		if( _value < _min )
		{
			return _min;
		}

		if( _value > _max )
		{
			return _max;
		}

		return _value;
	}

	MATH_INLINE bool cmp_f_f( float _a, float _b )
	{
		return (_a > (_b + mt::m_neps)) && (_a < (_b + mt::m_eps));
	}

	MATH_INLINE bool cmp_f_z( float _a )
	{
		return (_a > mt::m_neps) && (_a < mt::m_eps);
	}
}