#	include <math.h>

namespace mt
{
	MENGINE_MATH_FUNCTION_INLINE bool ccd_sphere_sphere( const mt::vec3f & _center1, float _radius1, const mt::vec3f & _velocity1, const mt::vec3f & _center2, float _radius2, const mt::vec3f & _velocity2, float & _time, mt::vec3f & _normal )
	{
		mt::vec3f nPos = _center1 - _center2;
		mt::vec3f nSpeed = _velocity1 - _velocity2;

		float nSpeedLen = mt::length_v3( nSpeed );
		
		if( nSpeedLen <= 0.f )
		{
			return false;
		}

		mt::vec3f nSpeedDir = nSpeed / nSpeedLen;

		float dist = mt::dot_v3_v3( -nPos, nSpeedDir );

		if( dist <= 0.f )
		{
			return false;
		}

		float nPosLen = mt::length_v3( nPos );

		float r = _radius1 + _radius2;

		float x = nPosLen * nPosLen - dist * dist;

		if( x > r * r )
		{
			return false;
		}

		float z = dist - ::sqrtf( r * r - x );

		_time = z / nSpeedLen;
		mt::norm_v3( _normal, nPos + nSpeedDir * z );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool ccd_sphere_plane( const mt::vec3f & _center, float _radius, const mt::vec3f & _velocity, const mt::planef & _plane, float & _time, mt::vec3f & _normal )
	{
		mt::vec3f dir;
		float speed = mt::norm_v3_f( dir, _velocity );

		mt::vec3f plane_point;
		if( mt::projection_to_plane( plane_point, _center, dir, _plane ) == false )
		{
			return false;
		}

		float dist = mt::length_v3_v3( _center, plane_point );

		_time = (dist - _radius) / speed;

		mt::reflect_plane( _normal, dir, _plane );

		return true;
	}
}