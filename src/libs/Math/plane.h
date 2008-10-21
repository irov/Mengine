#	pragma once

#	include "config.h"

#	include "vec3.h"

namespace mt
{
	class planef
	{
	public:
		union
		{
			/*struct
			{
				vec3f norm;
				float dist;
			};*/
			struct
			{
				float x,y,z;
				float d;
			};
			float v[4];
		};

		MATH_INLINE planef();
		MATH_INLINE planef(const planef& plane);
		MATH_INLINE planef(const vec3f &_norm, float _dist);
		MATH_INLINE planef(float _x, float _y, float _z, float _d);
	};

	MATH_INLINE void set_plane_by_triangle( planef & _plane, const vec3f _v[3] );
	MATH_INLINE void projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane );
}

#	if MATH_FORCE_INLINE == 1
#	include "plane_inline.h"
#	endif