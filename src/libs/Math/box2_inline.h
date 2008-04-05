namespace mt
{
	MATH_INLINE box2f::box2f()
	{}

	MATH_INLINE box2f::box2f( const box2f & _box )
		: vb( _box.vb )
		, ve( _box.ve )
	{}

	MATH_INLINE box2f::box2f( const vec2f & _vb, const vec2f & _ve )
		: vb( _vb )
		, ve( _ve )
	{}

	MATH_INLINE void reset( box2f & box, const vec2f &initValue)
	{
		box.max = initValue;
		box.min = initValue;
	}

	MATH_INLINE void reset( box2f & box, float x, float y)
	{
		reset(box,vec2f(x,y));
	}

	MATH_INLINE void add_internal_point( box2f & box, float x,float y)
	{
		if (x>box.max.x) box.max.x = x;
		if (y>box.max.y) box.max.y = y;
		if (x<box.min.x) box.min.x = x;
		if (y<box.min.y) box.min.y = y;
	}

	MATH_INLINE void add_internal_point( box2f & box, const vec2f &p)
	{
		add_internal_point(box,p.x, p.y);
	}

	MATH_INLINE void set_box_from_min_max( box2f & box, const vec2f &min, const vec2f &max)
	{
		box.vb = min;
		box.ve = max;
	}

	MATH_INLINE void set_box_from_center_and_extent( box2f & box, const vec2f &center, const vec2f &extent)
	{
		box.vb = center - extent;
		box.ve = center + extent;
	}

	MATH_INLINE void set_box_from_oriented_extent( box2f & box, const mt::vec2f & _offset, const mt::vec2f& _size, const mt::mat3f& _wm )
	{
		mt::vec2f bounds[4];

		bounds[0].x = _offset.x;
		bounds[0].y = _offset.y;

		bounds[1].x = _offset.x + _size.x;
		bounds[1].y = _offset.y;

		bounds[2].x = _offset.x;
		bounds[2].y = _offset.y + _size.y;

		bounds[3].x = _offset.x + _size.x;
		bounds[3].y = _offset.y + _size.y;

		mt::mul_v2_m3( box.min, bounds[0], _wm );

		box.max = box.min;

		mt::vec2f temp;
			
		for( unsigned int i = 1; i < 4; ++i)
		{
			mt::mul_v2_m3(temp, bounds[i], _wm );

			if ( box.max.x < temp.x ) box.max.x = temp.x;
			if ( box.max.y < temp.y ) box.max.y = temp.y;

			if ( box.min.x > temp.x ) box.min.x = temp.x;
			if ( box.min.y > temp.y ) box.min.y = temp.y;
		}
	}

	MATH_INLINE bool is_intersect( const box2f & _a, const box2f & _b )
	{
		return (_a.ve.y > _b.vb.y && _a.vb.y < _b.ve.y &&
				_a.ve.x > _b.vb.x && _a.vb.x < _b.ve.x);
	}
}