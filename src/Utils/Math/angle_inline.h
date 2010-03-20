#	include	"angle.h"
#	include	"simplex.h"
#	include <assert.h>

namespace mt
{
	MATH_INLINE float angle_norm360(float _angle)
	{
		if ((_angle >= 360.0f) || (_angle < 0.0f))
		{
			_angle -= floor( _angle / 360.0f ) * 360.0f;
		}
		return _angle;
	}

	MATH_INLINE float angle_norm180(float _angle)
	{
		_angle = angle_norm360(_angle);

		if (_angle > 180.0f) 
		{
			_angle -= 360.0f;
		}

		return _angle;
	}

	MATH_INLINE float angle_delta_deg(float _angle1, float _angle2)
	{
		return angle_norm180(_angle1 - _angle2);
	}

	MATH_INLINE float acos32(float _x)
	{
		if (_x <= -1.0f) 
		{
			return mt::m_pi;
		}

		if (_x >= 1.0f) 
		{
			return 0.0f;
		}

		return ::acosf(_x);
	}

	MATH_INLINE void direction( mt::vec2f & _vec, float _angle )
	{
		float cos_angle = cosf(_angle);
		float sin_angle = sinf(_angle);

		_vec.x = cos_angle;
		_vec.y = sin_angle;
	}

	MATH_INLINE float signed_angle(const mt::vec2f & _vec)
	{
		float len = _vec.length();

		if( len < 0.00001f )
		{
			assert(!"vec with zero length! ");
			return 0.f;
		}

		float cos = _vec.x / len;

		float x = mt::acos32( cos );

		if (_vec.y < 0)
		{
			return -x;
		}

		return x;
	}

	MATH_INLINE float angle_in_interval_deg( float _angle, float _min, float _max )
	{
		float delta = mt::angle_delta_deg(_max,_min);
		float delta1 = mt::angle_delta_deg(_angle,_min);

		return ((delta >= delta1) && (delta1 >= 0));
	}
}
