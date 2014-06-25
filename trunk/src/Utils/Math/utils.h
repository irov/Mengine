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
}