namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE planef::planef()
	{
	}

	MATH_INLINE planef::planef(const planef& plane)
		: norm(plane.norm)
		, dist(plane.dist)
	{
	}

	MATH_INLINE planef::planef(const vec3f &_norm, float _dist)
		: norm(_norm)
		, dist(_dist)
	{
	}

	MATH_INLINE planef::planef(float _x, float _y, float _z, float _d)
		: x(_x)
		, y(_y)
		, z(_z)
		, d(_d)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE void set_plane_by_triangle( planef & _plane, const vec3f _v[3] )
	{
		vec3f edge0 = _v[2] - _v[0];
		vec3f edge1 = _v[1] - _v[0];
		cross_v3_v3_norm( _plane.norm, edge0, edge1 );
		_plane.dist = dot_v3_v3( _v[0], _plane.norm );
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE void projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane )
	{
		_out = _dir * ( 
			dot_v3_v3( _plane.norm, _eye ) - _plane.dist ) / 
			dot_v3_v3( _plane.norm , _dir ) + _eye; 
	}
}