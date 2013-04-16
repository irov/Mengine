#	pragma once

#	include "config.h"

namespace mt
{
	template< typename T >
	MATH_FUNCTION_INLINE T clamp( const T& _leftBound, const T& _value, const T& _rightBound )
	{
		T clampedValue = _value;
		if( _value < _leftBound )
		{
			clampedValue = _leftBound;
		}
		else if( _value > _rightBound )
		{
			clampedValue = _rightBound;
		}
		return clampedValue;
	}
}	// namespace mt
