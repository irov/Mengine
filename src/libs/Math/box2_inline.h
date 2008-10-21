namespace mt
{
	MATH_INLINE box2f::box2f()
	{}

	MATH_INLINE box2f::box2f( const box2f & _box )
		: min( _box.min )
		, max( _box.max )
	{}

	MATH_INLINE box2f::box2f( const vec2f & _min, const vec2f & _max )
		: min( _min )
		, max( _max )
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

	MATH_INLINE void merge_box( box2f & box, const box2f & external_box )
	{
		if ( external_box.max.x > box.max.x ) box.max.x = external_box.max.x;
		if ( external_box.max.y > box.max.y ) box.max.y = external_box.max.y;
		if ( external_box.min.x < box.min.x ) box.min.x = external_box.min.x;
		if ( external_box.min.y < box.min.y ) box.min.y = external_box.min.y;
	}

	MATH_INLINE bool exist_box( const box2f & box, const box2f & external_box )
	{
		bool exist = true;

		if ( external_box.max.x > box.max.x ) exist = false;
		if ( external_box.max.y > box.max.y ) exist = false;
		if ( external_box.max.x < box.min.x ) exist = false;
		if ( external_box.max.y < box.min.y ) exist = false;

		return exist;
	}

	MATH_INLINE void set_box_from_min_max( box2f & box, const vec2f &min, const vec2f &max)
	{
		box.min = min;
		box.max = max;
	}

	MATH_INLINE void set_box_from_center_and_extent( box2f & box, const vec2f &center, const vec2f &extent)
	{
		box.min = center - extent;
		box.max = center + extent;
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
		return (_a.max.y > _b.min.y && _a.min.y < _b.max.y &&
				_a.max.x > _b.min.x && _a.min.x < _b.max.x);
	}

	MATH_INLINE vec2f box_size( const box2f& _box )
	{
		return _box.max - _box.min;
	}
}