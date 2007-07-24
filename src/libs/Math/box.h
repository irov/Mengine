#	pragma once

#	include "plane.h"

namespace mt
{
	class boxf
	{
	public:
		union
		{
			struct {vec3f vb,ve;};
			struct {vec3f v[2];};
			struct {vec3f MinEdge,MaxEdge;};
		};

	public:

		boxf()
		{}

		boxf(const boxf & _box)
			:	vb(_box.vb)
			,	ve(_box.ve)
		{}


		boxf(const vec3f &_vb,const vec3f &_ve)
			:	vb(_vb)
			,	ve(_ve)
		{}
	};

	void reset(boxf & box,const vec3f & initValue);
	void reset(boxf & box,float x, float y, float z);
	void add_internal_point(boxf & box, float x,float y,float z);
	void add_internal_point(boxf & box, const vec3f & p);
	void set_box_from_min_max(boxf & box, const vec3f & min, const vec3f & max);
	void set_box_from_center_and_extent(boxf & box, const vec3f & center, const vec3f & extent);
}