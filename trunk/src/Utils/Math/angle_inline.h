#	include	"angle.h"
#	include	"simplex.h"

namespace mt
{
	MATH_FUNCTION_INLINE float angle_norm(float _angle)
	{
		const float pi2 = mt::m_pi * 2.f;
		
		if( _angle < 0.f )
		{
			float pi_count = floorf(_angle / pi2);
			float pi_abs = pi_count * pi2;

			_angle -= pi_abs;
		}
		
		if( _angle > pi2 )
		{
			float pi_count = ceilf(_angle / pi2);
			float pi_abs = pi_count * pi2;

			_angle -= pi_abs;
		}

		return _angle;
	}

	MATH_FUNCTION_INLINE void angle_correct_interpolate_from_to(float _from, float _to, float & _correct_from, float & _correct_to)
	{
		float norm_angle_from = mt::angle_norm(_from);
		float norm_angle_to = mt::angle_norm(_to);

		float correct_angle = norm_angle_to;

		if( norm_angle_to > norm_angle_from )
		{
			float dist = norm_angle_to - norm_angle_from;

			if( (norm_angle_from + mt::m_pi * 2.f) - norm_angle_to < dist )
			{
				correct_angle = norm_angle_to - mt::m_pi * 2.f;
			}
		}
		else
		{
			float dist = norm_angle_from - norm_angle_to;

			if( (norm_angle_to + mt::m_pi * 2.f) - norm_angle_from < dist )
			{
				correct_angle = norm_angle_to + mt::m_pi * 2.f;
			}
		}

		_correct_from = norm_angle_from;
		_correct_to = correct_angle;
	}

	MATH_FUNCTION_INLINE float angle_length( float _angle1, float _angle2 )
	{
		float correct_angle_from;
		float correct_angle_to;
		mt::angle_correct_interpolate_from_to( _angle1, _angle2, correct_angle_from, correct_angle_to );

		float length = correct_angle_to - correct_angle_from;

		return length;
	}

	MATH_FUNCTION_INLINE float angle_norm360(float _angle)
	{
		if ((_angle >= 360.f) || (_angle < 0.f))
		{
			_angle -= floorf( _angle / 360.f ) * 360.f;
		}

		return _angle;
	}

	MATH_FUNCTION_INLINE float angle_norm180(float _angle)
	{
		_angle = angle_norm360(_angle);

		if (_angle > 180.f) 
		{
			_angle -= 360.f;
		}

		return _angle;
	}

	MATH_FUNCTION_INLINE float angle_delta_deg(float _angle1, float _angle2)
	{
		return angle_norm180(_angle1 - _angle2);
	}

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

	MATH_FUNCTION_INLINE void direction( mt::vec2f & _vec, float _angle )
	{
		float cos_angle = cosf(_angle);
		float sin_angle = sinf(_angle);

		_vec.x = cos_angle;
		_vec.y = sin_angle;
	}

	MATH_FUNCTION_INLINE float signed_angle(const mt::vec2f & _vec)
	{
		float len = _vec.length();

		if( len < 0.00001f )
		{			
			return 0.f;
		}

		float cos = _vec.x / len;

		float x = mt::acos32( cos );

		if( _vec.y > 0.f )
		{
			return -x;
		}

		return x;
	}

	MATH_FUNCTION_INLINE float angle_in_interval_deg( float _angle, float _min, float _max )
	{
		float delta = mt::angle_delta_deg(_max,_min);
		float delta1 = mt::angle_delta_deg(_angle,_min);

		return ((delta >= delta1) && (delta1 >= 0.f));
	}

	MATH_FUNCTION_INLINE void make_euler_angles( mt::vec3f & _euler, const mat4f & _rotate )
	{
		float sinY = _rotate.v2.x;

		float x;
		float y;
		float z;

		if( fabsf( fabsf( sinY ) - 1.f ) < m_eps )
		{
			x = 0.f;

			if( fabsf( sinY + 1.f ) < m_eps )
			{
				y = m_half_pi;
				z = x + atan2f( _rotate.v0.y, _rotate.v0.z );
			}
			else
			{
				y = -m_half_pi;
				z = -x + atan2f( -_rotate.v0.y, -_rotate.v0.z );
			}
		}
		else
		{
			y = -asinf( sinY );
			float cosY = cosf( y );

			x = atan2f( _rotate.v1.x / cosY, _rotate.v0.x / cosY );
			z = atan2f( _rotate.v2.y / cosY, _rotate.v2.z / cosY );
		}

		_euler.x = x;
		_euler.y = z;
		_euler.z = y;
	}
}
