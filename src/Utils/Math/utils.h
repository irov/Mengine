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
	MENGINE_MATH_INLINE bool equal_f_f_e( float _a, float _b, float _e )
	{
		return (_a >= (_b - _e)) && (_a <= (_b + _e));
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool equal_f_f( float _a, float _b )
	{
		return (_a >= (_b + mt::m_neps)) && (_a <= (_b + mt::m_eps));
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool equal_f_z( float _f )
	{
		return (_f >= mt::m_neps) && (_f <= mt::m_eps);
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool equal_f_1( float _f )
	{
		return (_f >= mt::m_1_eps) && (_f <= mt::m_1_neps);
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE bool greatequal_f_z( float _f )
	{
		return _f >= mt::m_neps;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE unsigned int factorial( unsigned int _value )
	{
		if( _value == 0 )
		{
			return 1;
		}
		
		unsigned int f = 1;
		unsigned int d = 0;
		
		for( unsigned int i = 0; i != _value; ++i )
		{			
			++d;

			f *= d;
		}

		return f;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE float factorialf( unsigned int _value )
	{
		if( _value == 0 )
		{
			return 1.f;
		}

		float f = 1.f;
		float d = 0.f;

		for( unsigned int i = 0; i != _value; ++i )
		{
			d += 1.f;

			f *= d;
		}

		return f;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE float integral_powf( float _value, unsigned int _count )
	{
		if( _count == 0 )
		{
			return 1.f;
		}

		float f = _value;

		for( unsigned int i = 1; i != _count; ++i )
		{
			f *= _value;
		}

		return f;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE float length_f( const float & _a, const float & _b )
	{
		float d = _a - _b;

		if( d >= 0.f )
		{
			return d;
		}

		return -d;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE float length( const float & _a, const float & _b )
	{
		return length_f( _a, _b );
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_INLINE void linerp_f1( float & _out, float _in1, float _in2, float _scale )
	{
		_out = _in1 + (_in2 - _in1) * _scale;
	}
}