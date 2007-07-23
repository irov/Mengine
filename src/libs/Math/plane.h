#	pragma once

#	include "vec3.h"

namespace mt
{
	class planef
	{
	public:
		union
		{
			struct
			{
				vec3f norm;
				float dist;
			};
			struct
			{
				float x,y,z;
				float d;
			};
			float v[4];
		};

		planef()
		{
		}

		planef(const planef& plane)
		: norm(plane.norm)
		, dist(plane.dist)
		{
		}

		planef(const vec3f &_norm, float _dist)
		: norm(_norm)
		, dist(_dist)
		{
		}

		planef(float _x, float _y, float _z, float _d)
		: x(_x)
		, y(_y)
		, z(_z)
		, d(_d)
		{
		}
	};

	void set_plane_by_triangle( planef & _plane, const vec3f _v[3] );
	void projection_to_plane( vec3f & _out, const vec3f & _eye, const vec3f  & _dir, const planef & _plane );
}

