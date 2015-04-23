#	pragma once

#	include "config.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE float max_f( float _x, float _y )
	{
		return ( _x > _y ) ? _x : _y;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	MENGINE_MATH_INLINE T ltrim_f( T _value, T _trim )
	{
		if( _value < _trim )
		{
			return _trim;
		}

		return _value;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	MENGINE_MATH_INLINE T rtrim_f( T _value, T _trim )
	{
		if( _value > _trim )
		{
			return _trim;
		}

		return _value;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	MENGINE_MATH_INLINE T crop( T _value, T _min, T _max )
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
	//////////////////////////////////////////////////////////////////////////
	template< typename T >
	MENGINE_MATH_FUNCTION_INLINE T clamp( const T & _leftBound, const T & _value, const T & _rightBound )
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
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool equal_f_f( float _a, float _b )
	{
		return (_a > (_b + mt::m_neps)) && (_a < (_b + mt::m_eps));
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool equal_f_z( float _f )
	{
		return (_f > mt::m_neps) && (_f < mt::m_eps);
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool equal_f_1( float _f )
	{
		return (_f > mt::m_1_eps) && (_f < mt::m_1_neps);
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool greatequal_f_z( float _f )
	{
		return _f >= mt::m_neps;
	}
}