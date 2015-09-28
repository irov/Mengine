#	include "utils.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE planef::planef()
		:a( 1.f )
		, b( 0.f )
		, c( 0.f )
		, d( 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE planef::planef( const planef& _plane )
		:a( _plane.a )
		, b( _plane.b )
		, c( _plane.c )
		, d( _plane.d )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE planef::planef( const vec3f &_norm, float _dist )
		:a( _norm.x )
		, b( _norm.y )
		, c( _norm.z )
		, d( _dist )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE planef::planef( float _a, float _b, float _c, float _d )
		: a( _a )
		, b( _b )
		, c( _c )
		, d( _d )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE float * planef::buff()
	{
		return &a;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE const float * planef::buff() const
	{
		return &a;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void get_plane_normal( vec3f & _out, const planef & _plane )
	{
		_out.x = _plane.a;
		_out.y = _plane.b;
		_out.z = _plane.c;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void get_plane_point( vec3f & _out, const planef & _plane )
	{
		_out.x = - _plane.a * _plane.d;
		_out.y = - _plane.b * _plane.d;
		_out.z = - _plane.c * _plane.d;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void set_plane_by_triangle( planef & _plane, const vec3f & _v0, const vec3f & _v1, const vec3f & _v2 )
	{
		vec3f edge0 = _v1 - _v0;
		vec3f edge1 = _v2 - _v0;

		vec3f norm;
		cross_v3_v3_norm( norm, edge0, edge1 );

		set_plane_by_point_and_normal( _plane, _v0, norm );
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void set_plane_by_point_and_normal( planef & _plane, const vec3f & _point, const vec3f & _normal )
	{
		_plane.a = _normal.x;
		_plane.b = _normal.y;
		_plane.c = _normal.z;
		_plane.d = -dot_v3_v3( _point, _normal );
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane )
	{
		vec3f norm( _plane.a, _plane.b, _plane.c );

		float dot_norm_eye = dot_v3_v3( norm, _eye );

		if( mt::equal_f_z( dot_norm_eye ) == true )
		{
			return false;
		}

		_out = _dir * (dot_norm_eye - _plane.d) / dot_v3_v3( norm, _dir ) + _eye;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void reflect_plane( vec3f & _out, const vec3f & _dir, const planef & _plane )
	{
		vec3f norm( _plane.a, _plane.b, _plane.c );

		mt::reflect_v3_v3( _out, norm, _dir );
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void mul_plane_mat4( planef & _out, const planef & _plane, const mt::mat4f & _wm )
	{
		mt::vec3f dir;
		mt::get_plane_normal( dir, _plane );

		mt::vec3f pos;
		mt::get_plane_point( pos, _plane );

		mt::vec3f at = pos + dir;

		mt::vec3f pos_wm;
		mt::mul_v3_m4( pos_wm, pos, _wm );

		mt::vec3f at_wm;
		mt::mul_v3_m4( at_wm, at, _wm );

		mt::vec3f dir_wm;
		mt::dir_v3_v3( dir_wm, at_wm, pos_wm );
		
		set_plane_by_point_and_normal( _out, pos_wm, dir_wm );		
	}
}
