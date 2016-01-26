#	include <math.h>

#	include "utils.h"

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
		mt::norm_v3_v3( _normal, nPos + nSpeedDir * z );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool ccd_ray_plane( const mt::vec3f & _point, const mt::vec3f & _velocity, const mt::planef & _plane, float & _time )
	{
		mt::vec3f pn;
		mt::get_plane_normal( pn, _plane );

		float denom = mt::dot_v3_v3( _velocity, pn );

		if( mt::greatequal_f_z( denom ) == true )
		{
			return false;
		}
		
		float orgon = mt::dot_v3_v3( _point, pn );

		float numer = -(_plane.d + orgon);

		_time = numer / denom;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool ccd_sphere_plane( const mt::vec3f & _center, float _radius, const mt::vec3f & _velocity, const mt::planef & _plane, float & _time )
	{
		mt::vec3f pn;
		mt::get_plane_normal( pn, _plane );

		mt::vec3f pt = _center - pn * _radius;

		bool successful = mt::ccd_ray_plane( pt, _velocity, _plane, _time );

		return successful;
	}
}