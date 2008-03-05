#	pragma once

#	include <math.h>
#	include <assert.h>

namespace mt
{
	/* –авномерное распределение с a != b */
	float	even_rand( float a, float b )
	{
		assert( b == a );
		float alpha = (float)rand() / RAND_MAX;
		return a + alpha * ( b - a );
	}

	/* Ёксп. распределение с lambda > 0*/
	float	exp_rand( float lambda )
	{
		assert( lambda );
		float alpha = (float)(rand() + 1) / RAND_MAX;
		float inv_lambda = 1.0f / lambda;
		return inv_lambda * log( 1.0f / ( 1.0f - alpha ) );
	}
}