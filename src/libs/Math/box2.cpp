#	include	"box2.h"	

namespace mt
{
	void reset(box2f &box,const vec2f &initValue)
	{
		box.MaxEdge = initValue;
		box.MinEdge = initValue;
	}

	void reset(box2f &box,float x, float y)
	{
		reset(box,vec2f(x,y));
	}

	void add_internal_point(box2f &box, float x,float y)
	{
		if (x>box.MaxEdge.x) box.MaxEdge.x = x;
		if (y>box.MaxEdge.y) box.MaxEdge.y = y;
		if (x<box.MinEdge.x) box.MinEdge.x = x;
		if (y<box.MinEdge.y) box.MinEdge.y = y;
	}

	void add_internal_point(box2f &box, const vec2f &p)
	{
		add_internal_point(box,p.x, p.y);
	}

	void set_box_from_min_max(box2f &box, const vec2f &min, const vec2f &max)
	{
		box.vb = min;
		box.ve = max;
	}

	void set_box_from_center_and_extent(box2f &box, const vec2f &center, const vec2f &extent)
	{
		box.vb = center - extent;
		box.ve = center + extent;
	}

	bool is_intersect(const box2f & _a, const box2f & _b)
	{
		return (_a.ve.y > _b.vb.y && _a.vb.y < _b.ve.y &&
				_a.ve.x > _b.vb.x && _a.vb.x < _b.ve.x);
	}
}