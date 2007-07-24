#	include	"box.h"	

namespace mt
{
	void reset(boxf &box,const vec3f &initValue)
	{
		box.MaxEdge = initValue;
		box.MinEdge = initValue;
	}

	void reset(boxf &box,float x, float y, float z)
	{
		reset(box,vec3f(x,y,z));
	}

	void add_internal_point(boxf &box, float x,float y,float z)
	{
		if (x>box.MaxEdge.x) box.MaxEdge.x = x;
		if (y>box.MaxEdge.y) box.MaxEdge.y = y;
		if (z>box.MaxEdge.z) box.MaxEdge.z = z;
		if (x<box.MinEdge.x) box.MinEdge.x = x;
		if (y<box.MinEdge.y) box.MinEdge.y = y;
		if (z<box.MinEdge.z) box.MinEdge.z = z;
	}

	void add_internal_point(boxf &box, const vec3f &p)
	{
		add_internal_point(box,p.x, p.y, p.z);
	}

	void set_box_from_min_max(boxf &box, const vec3f &min, const vec3f &max)
	{
		box.vb = min;
		box.ve = max;
	}

	void set_box_from_center_and_extent(boxf &box, const vec3f &center, const vec3f &extent)
	{
		box.vb = center - extent;
		box.ve = center + extent;
	}

	void GetUVHVector(vec3f uvh[3],const boxf &box)
	{
		vec3f vDir = box.ve - box.vb;
		uvh[0] = vec3f(vDir.x,0,0);
		uvh[1] = vec3f(0,vDir.y,0);
		uvh[2] = vec3f(0,0,vDir.z);
	}

	void get_point_from_box(vec3f point[8],const boxf &box)
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
}