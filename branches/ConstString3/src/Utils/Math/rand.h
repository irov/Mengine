#	pragma once

#	include "config.h"

//#	include <math.h>
#	include <cassert>
#	include <cstdlib>


namespace mt
{
	MATH_INLINE float randf( float _max )
	{
		float rnf = static_cast<float>( std::rand() ) / ( static_cast<float>(RAND_MAX) + 1.0f);
		float rf = _max * rnf;
		return rf;
	}

	/* –авномерное распределение с a != b */
	MATH_INLINE float range_randf( float _a, float _b )
	{
		if( _a > _b )
		{
			return _a;
		}

		float max = ( _b - _a );
		float rf = randf( max );

		return _a + rf;
	}

	MATH_INLINE int rand( int _max )
	{
		float rf = randf( static_cast<float>(_max) );
		int ri = static_cast<int>( rf );
		return ri;
	}

	MATH_INLINE int range_rand( int _a, int _b )
	{
		if( _a > _b )
		{
			return _a;
		}

		int max = ( _b - _a );
		int ri = mt::rand( max );
		return _a + ri;
	}

	/* Ёксп. распределение с lambda > 0*/
	//MATH_INLINE float exp_rand( float lambda )
	//{
	//	assert( lambda );
	//	float alpha = static_cast<float>(::rand() + 1) / RAND_MAX; ---- WTF?? Maybe static_cast<float>(::rand()) / ( RAND_MAX + 1 )
	//	float inv_lambda = 1.0f / lambda;
	//	return inv_lambda * logf( 1.0f / ( 1.0f - alpha ) );
	//}
}
