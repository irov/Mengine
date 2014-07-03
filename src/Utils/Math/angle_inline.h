#	include <math.h>
#	include <stdio.h>

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_norm(float _angle)
	{
		if( _angle < 0.f )
		{
			float pi_count = floorf(_angle * mt::m_inv_two_pi);
			float pi_abs = pi_count * mt::m_two_pi;

			_angle -= pi_abs;
		}
		
		if( _angle > mt::m_two_pi )
		{
			float pi_count = floorf(_angle * mt::m_inv_two_pi);
			float pi_abs = pi_count * mt::m_two_pi;

			_angle -= pi_abs;
		}

		return _angle;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_norm2(float _angle)
	{
		float angle2 = mt::angle_norm( _angle + mt::m_pi );
		
		angle2 -= mt::m_pi;

		return angle2;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void angle_correct_interpolate_from_to(float _from, float _to, float & _correct_from, float & _correct_to)
	{
		float norm_angle_from = mt::angle_norm(_from);
		float norm_angle_to = mt::angle_norm(_to);

		float correct_angle = norm_angle_to;

		if( norm_angle_to > norm_angle_from )
		{
			float dist = norm_angle_to - norm_angle_from;

			if( (norm_angle_from + mt::m_two_pi) - norm_angle_to < dist )
			{
				correct_angle = norm_angle_to - mt::m_two_pi;
			}
		}
		else
		{
			float dist = norm_angle_from - norm_angle_to;

			if( (norm_angle_to + mt::m_two_pi) - norm_angle_from < dist )
			{
				correct_angle = norm_angle_to + mt::m_two_pi;
			}
		}

		_correct_from = norm_angle_from;
		_correct_to = correct_angle;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_length( float _angle1, float _angle2 )
	{
		float correct_angle_from;
		float correct_angle_to;
		mt::angle_correct_interpolate_from_to( _angle1, _angle2, correct_angle_from, correct_angle_to );

		float length = correct_angle_to - correct_angle_from;

		return length;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_norm360(float _angle)
	{
		if ((_angle >= 360.f) || (_angle < 0.f))
		{
			_angle -= floorf( _angle / 360.f ) * 360.f;
		}

		return _angle;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_norm180(float _angle)
	{
		_angle = angle_norm360(_angle);

		if (_angle > 180.f) 
		{
			_angle -= 360.f;
		}

		return _angle;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_delta_deg(float _angle1, float _angle2)
	{
		return angle_norm180(_angle1 - _angle2);
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float acos32(float _x)
	{
		if (_x <= -1.f) 
		{
			return mt::m_pi;
		}

		if (_x >= 1.f) 
		{
			return 0.f;
		}

		return ::acosf(_x);
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float angle_in_interval_deg( float _angle, float _min, float _max )
	{
		float delta = mt::angle_delta_deg(_max,_min);
		float delta1 = mt::angle_delta_deg(_angle,_min);

		return ((delta >= delta1) && (delta1 >= 0.f));
	}
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float cosf_fast( float x )
	{
		x = mt::angle_norm2( x + mt::m_half_pi );

		float cos = sinf_fast_pi_pi( x );

		return cos;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float sinf_fast( float x )
	{
		float x2 = mt::angle_norm2( x );

		float sin = sinf_fast_pi_pi( x2 );

		return sin;
	}
}
