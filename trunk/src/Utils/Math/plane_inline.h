#	include "utils.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE planef::planef()
		:x( 1.f )
		, y( 0.f )
		, z( 0.f )
		, d( 0.f )
	{
	}

	MENGINE_MATH_METHOD_INLINE planef::planef( const planef& plane )
		:x( plane.x )
		, y( plane.y )
		, z( plane.z )
		, d( plane.d )
	{
	}

	MENGINE_MATH_METHOD_INLINE planef::planef( const vec3f &_norm, float _dist )
		:x( _norm.x )
		, y( _norm.y )
		, z( _norm.z )
		, d( _dist )
		//: norm(_norm)
		//, dist(_dist)
	{
	}

	MENGINE_MATH_METHOD_INLINE planef::planef( float _x, float _y, float _z, float _d )
		: x( _x )
		, y( _y )
		, z( _z )
		, d( _d )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void set_plane_by_triangle( planef & _plane, const vec3f & _v0, const vec3f & _v1, const vec3f & _v2 )
	{
		vec3f edge0 = _v2 - _v0;
		vec3f edge1 = _v1 - _v0;
		vec3f norm;

		cross_v3_v3_norm( norm, edge0, edge1 );

		_plane.x = norm.x;
		_plane.y = norm.y;
		_plane.z = norm.z;
		_plane.d = dot_v3_v3( _v0, norm );
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane )
	{
		vec3f norm( _plane.x, _plane.y, _plane.z );

		float dot_norm_eye = dot_v3_v3( norm, _eye );

		if( mt::cmp_f_z( dot_norm_eye ) == true )
		{
			return false;
		}

		_out = _dir * (dot_norm_eye - _plane.d) / dot_v3_v3( norm, _dir ) + _eye;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void reflect_plane( vec3f & _out, const vec3f & _dir, const planef & _plane )
	{
		vec3f norm( _plane.x, _plane.y, _plane.z );

		mt::reflect_v3_v3( _out, norm, _dir );
	}
}
