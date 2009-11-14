#	pragma once

#	include "config.h"

#	include <math.h>
#	include <assert.h>

namespace mt
{
	MATH_INLINE unsigned int rand( unsigned int _max )
	{
		unsigned int r = static_cast<unsigned int>( float(_max) * ::rand() / (RAND_MAX+1) );
		return r;
	}

	MATH_INLINE int range_rand( int _min, int _max )
	{
		unsigned int r = rand( _max - _min );
		return _min + r;
	}

	/* –авномерное распределение с a != b */
	MATH_INLINE float even_rand( float a, float b )
	{
		assert( b != a );
		float alpha = static_cast<float>(::rand()) / RAND_MAX;
		return a + alpha * ( b - a );
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
