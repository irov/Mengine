#	pragma once

#	include "config.h"

#	include <math.h>

namespace mt
{
	MATH_FUNCTION_INLINE float sinf_fast_pi_pi( float x )
	{
		float sin;

		if( x < 0.f )
		{
			sin = 1.27323954f * x + 0.405284735f * x * x;

			if( sin < 0.f )
			{
				sin = 0.225f * (sin * -sin - sin) + sin;
			}
			else
			{
				sin = 0.225f * (sin * sin - sin) + sin;
			}
		}
		else
		{
			sin = 1.27323954f * x - 0.405284735f * x * x;

			if( sin < 0.f )
			{
				sin = 0.225f * (sin * -sin - sin) + sin;
			}
			else
			{
				sin = 0.225f * (sin * sin - sin) + sin;
			}
		}

		return sin;
	}

	MATH_FUNCTION_INLINE float cosf_fast( float x )
	{
		if( x < -6.28318531f || x > 6.28318531f )
		{
			return cosf( x );
		}

		if (x < -3.14159265f)
		{
			x += 6.28318531f;
		}
		else
		{
			if( x >  3.14159265f )
			{
				x -= 6.28318531f;
			}
		}

		x += 1.57079632f;

		if( x >  3.14159265f )
		{
			x -= 6.28318531f;
		}

		float cos = sinf_fast_pi_pi( x );

		return cos;
	}

	MATH_FUNCTION_INLINE float sinf_fast( float x )
	{
		if( x < -6.28318531f || x > 6.28318531f )
		{
			return sinf( x );
		}

		if (x < -3.14159265f)
		{
			x += 6.28318531f;
		}
		else
		{
			if( x >  3.14159265f )
			{
				x -= 6.28318531f;
			}
		}

		float sin = sinf_fast_pi_pi( x );

		return sin;
	}
}