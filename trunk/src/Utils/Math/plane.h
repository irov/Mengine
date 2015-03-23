#	pragma once

#	include "config.h"

#	include "vec3.h"

namespace mt
{
	class planef
	{
	public:
        float x;
        float y;
        float z;
        float d;

		MENGINE_MATH_METHOD_INLINE planef();
		MENGINE_MATH_METHOD_INLINE planef(const planef& plane);
		MENGINE_MATH_METHOD_INLINE planef(const vec3f &_norm, float _dist);
		MENGINE_MATH_METHOD_INLINE planef(float _x, float _y, float _z, float _d);
	};

	MENGINE_MATH_FUNCTION_INLINE void set_plane_by_triangle( planef & _plane, const vec3f _v[3] );
	MENGINE_MATH_FUNCTION_INLINE void projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "plane_inline.h"
#	endif
