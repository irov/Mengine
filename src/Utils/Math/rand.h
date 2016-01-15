#	pragma once

#	include "config.h"

#	include <stdlib.h>

namespace mt
{
	MENGINE_MATH_INLINE float randf( float _max )
	{
		float rnf = static_cast<float>( ::rand() ) / ( static_cast<float>(RAND_MAX) + 1.0f);
		float rf = _max * rnf;
		return rf;
	}

	/* –авномерное распределение с a != b */
	MENGINE_MATH_INLINE float range_randf( float _a, float _b )
	{
		if( _a > _b )
		{
			return _a;
		}

		float max = ( _b - _a );
		float rf = randf( max );

		return _a + rf;
	}

	MENGINE_MATH_INLINE uint32_t rand( uint32_t _max )
	{
		float rf = randf( static_cast<float>(_max) );

		uint32_t ri = static_cast<uint32_t>(rf);

		return ri;
	}

	MENGINE_MATH_INLINE uint32_t range_rand( uint32_t _a, uint32_t _b )
	{
		if( _a > _b )
		{
			return _a;
		}

		uint32_t max = (_b - _a);
		uint32_t ri = mt::rand( max );

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
