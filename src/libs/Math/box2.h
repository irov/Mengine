#	pragma once

#	include "vec2.h"
#	include "mat3.h"

namespace mt
{
	struct box2f
	{
		union
		{
			struct {vec2f vb,ve;};
			struct {vec2f v[2];};
			struct {vec2f min,max;};
		};

		box2f();
		box2f( const box2f & _box );
		box2f( const vec2f & _vb, const vec2f & _ve );
	};

	void reset(box2f & box,const vec2f & initValue);
	void reset(box2f & box,float x, float y);
	void add_internal_point(box2f & box, float x,float y);
	void add_internal_point(box2f & box, const vec2f & p);
	void set_box_from_min_max(box2f & box, const vec2f & min, const vec2f & max);
	void set_box_from_center_and_extent(box2f & box, const vec2f & center, const vec2f & extent);
	void set_box_from_oriented_extent( box2f & box, const mt::vec2f & _size, const mt::mat3f & _wm );
	bool is_intersect(const box2f & _a, const box2f & _b);
}