#	pragma once

#	include "plane.h"

namespace mt
{
	struct box3f
	{
		union
		{
			struct {vec3f vb,ve;};
			struct {vec3f v[2];};
			struct {vec3f min, max;};
		};

		box3f()
		{}

		box3f(const box3f & _box)
			:	vb(_box.vb)
			,	ve(_box.ve)
		{}


		box3f(const vec3f &_vb,const vec3f &_ve)
			:	vb(_vb)
			,	ve(_ve)
		{}
	};

	void reset(box3f & box,const vec3f & initValue);
	void reset(box3f & box,float x, float y, float z);
	void add_internal_point(box3f & box, float x,float y,float z);
	void add_internal_point(box3f & box, const vec3f & p);
	void set_box_from_min_max(box3f & box, const vec3f & min, const vec3f & max);
	void set_box_from_center_and_extent(box3f & box, const vec3f & center, const vec3f & extent);
	void get_point_from_box(vec3f point[8],const box3f & box);
	bool is_intersect(const box3f & _a, const box3f & _b);
}