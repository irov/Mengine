namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MATH_METHOD_INLINE planef::planef()
	{
	}

	MATH_METHOD_INLINE planef::planef(const planef& plane)
		:x( plane.x )
		,y( plane.y )
		,z( plane.z )
		,d( plane.d )
		//: norm(plane.norm)
		//, dist(plane.dist)
	{
	}

	MATH_METHOD_INLINE planef::planef(const vec3f &_norm, float _dist)
		:x( _norm.x )
		,y( _norm.y )
		,z( _norm.z )
		,d( _dist )
		//: norm(_norm)
		//, dist(_dist)
	{
	}

	MATH_METHOD_INLINE planef::planef(float _x, float _y, float _z, float _d)
		: x(_x)
		, y(_y)
		, z(_z)
		, d(_d)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void set_plane_by_triangle( planef & _plane, const vec3f _v[3] )
	{
		vec3f edge0 = _v[2] - _v[0];
		vec3f edge1 = _v[1] - _v[0];
		vec3f norm;
		cross_v3_v3_norm( norm, edge0, edge1 );
		_plane.x = norm.x;
		_plane.y = norm.y;
		_plane.z = norm.z;
		_plane.d = dot_v3_v3( _v[0], norm );
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane )
	{
		vec3f norm( _plane.x, _plane.y, _plane.z );
		_out = _dir * ( 
			dot_v3_v3( norm, _eye ) - _plane.d ) / 
			dot_v3_v3( norm , _dir ) + _eye; 

	}
}
