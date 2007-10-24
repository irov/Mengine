#	include	"box3.h"	

namespace mt
{
	void reset(box3f &box,const vec3f &initValue)
	{
		box.max = initValue;
		box.min = initValue;
	}

	void reset(box3f &box,float x, float y, float z)
	{
		reset(box,vec3f(x,y,z));
	}

	void add_internal_point(box3f &box, float x,float y,float z)
	{
		if (x>box.max.x) box.max.x = x;
		if (y>box.max.y) box.max.y = y;
		if (z>box.max.z) box.max.z = z;
		if (x<box.min.x) box.min.x = x;
		if (y<box.min.y) box.min.y = y;
		if (z<box.min.z) box.min.z = z;
	}

	void add_internal_point(box3f &box, const vec3f &p)
	{
		add_internal_point(box,p.x, p.y, p.z);
	}

	void set_box_from_min_max(box3f &box, const vec3f &min, const vec3f &max)
	{
		box.vb = min;
		box.ve = max;
	}

	void set_box_from_center_and_extent(box3f &box, const vec3f &center, const vec3f &extent)
	{
		box.vb = center - extent;
		box.ve = center + extent;
	}

	void GetUVHVector(vec3f uvh[3],const box3f &box)
	{
		vec3f vDir = box.ve - box.vb;
		uvh[0] = vec3f(vDir.x,0,0);
		uvh[1] = vec3f(0,vDir.y,0);
		uvh[2] = vec3f(0,0,vDir.z);
	}

	void get_point_from_box(vec3f point[8],const box3f &box)
	{
		vec3f uvh[3];
		GetUVHVector(uvh,box);

		point[0] = box.vb;
		point[1] = box.vb + uvh[0];
		point[2] = box.vb + uvh[0] + uvh[1];
		point[3] = box.vb + uvh[1];
		point[4] = box.vb + uvh[2];
		point[5] = box.vb + uvh[0] + uvh[2];
		point[6] = box.vb + uvh[0] + uvh[1] + uvh[2];
		point[7] = box.vb + uvh[1] + uvh[2];
	}

	bool is_intersect(const box3f & _a, const box3f & _b)
	{
		return (_a.ve.y > _b.vb.y && _a.vb.y < _b.ve.y &&
				_a.ve.x > _b.vb.x && _a.vb.x < _b.ve.x &&
				_a.ve.z > _b.vb.z && _a.vb.z < _b.ve.z);
	}
}