namespace mt
{
	MATH_INLINE box3f::box3f()
	{}

	MATH_INLINE box3f::box3f(const box3f & _box)
		:	min(_box.min)
		,	max(_box.max)
	{}


	MATH_INLINE box3f::box3f(const vec3f &_min,const vec3f &_max)
		:	min(_min)
		,	max(_max)
	{}

	MATH_INLINE void reset(box3f &box,const vec3f &initValue)
	{
		box.max = initValue;
		box.min = initValue;
	}

	MATH_INLINE void reset(box3f &box,float x, float y, float z)
	{
		reset(box,vec3f(x,y,z));
	}

	MATH_INLINE void add_internal_point(box3f &box, float x,float y,float z)
	{
		if (x>box.max.x) box.max.x = x;
		if (y>box.max.y) box.max.y = y;
		if (z>box.max.z) box.max.z = z;
		if (x<box.min.x) box.min.x = x;
		if (y<box.min.y) box.min.y = y;
		if (z<box.min.z) box.min.z = z;
	}

	MATH_INLINE void add_internal_point(box3f &box, const vec3f &p)
	{
		add_internal_point(box,p.x, p.y, p.z);
	}

	MATH_INLINE void set_box_from_min_max(box3f &box, const vec3f &min, const vec3f &max)
	{
		box.min = min;
		box.max = max;
	}

	MATH_INLINE void set_box_from_center_and_extent(box3f &box, const vec3f &center, const vec3f &extent)
	{
		box.min = center - extent;
		box.max = center + extent;
	}

	MATH_INLINE void GetUVHVector(vec3f uvh[3],const box3f &box)
	{
		vec3f vDir = box.max - box.min;
		uvh[0] = vec3f(vDir.x,0,0);
		uvh[1] = vec3f(0,vDir.y,0);
		uvh[2] = vec3f(0,0,vDir.z);
	}

	MATH_INLINE void get_point_from_box(vec3f point[8],const box3f &box)
	{
		vec3f uvh[3];
		GetUVHVector(uvh,box);

		point[0] = box.min;
		point[1] = box.min + uvh[0];
		point[2] = box.min + uvh[0] + uvh[1];
		point[3] = box.min + uvh[1];
		point[4] = box.min + uvh[2];
		point[5] = box.min + uvh[0] + uvh[2];
		point[6] = box.min + uvh[0] + uvh[1] + uvh[2];
		point[7] = box.min + uvh[1] + uvh[2];
	}

	MATH_INLINE bool is_intersect(const box3f & _a, const box3f & _b)
	{
		return (_a.max.y > _b.min.y && _a.min.y < _b.max.y &&
				_a.max.x > _b.min.x && _a.min.x < _b.max.x &&
				_a.max.z > _b.min.z && _a.min.z < _b.max.z);
	}
}