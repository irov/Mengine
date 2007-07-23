#	include "plane.h"



namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	void set_plane_by_triangle( planef & _plane, const vec3f _v[3] )
	{
		vec3f edge0 = _v[2] - _v[0];
		vec3f edge1 = _v[1] - _v[0];
		cross_v3_v3_norm( _plane.norm, edge0, edge1 );
		_plane.dist = dot_v3_v3( _v[0], _plane.norm );
	}

	//////////////////////////////////////////////////////////////////////////
	void projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane )
	{
		_out = _dir * ( 
			dot_v3_v3( _plane.norm, _eye ) - _plane.dist ) / 
			dot_v3_v3( _plane.norm , _dir ) + _eye; 
	}
}