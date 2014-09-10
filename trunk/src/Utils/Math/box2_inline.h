#	include <math.h>

namespace mt
{
	MATH_METHOD_INLINE box2f::box2f()
	{}

	MATH_METHOD_INLINE box2f::box2f( const box2f & _box )
		: minimum(_box.minimum)
		, maximum(_box.maximum)
	{}

	MATH_METHOD_INLINE box2f::box2f( const vec2f & _min, const vec2f & _max )
		: minimum(_min)
		, maximum(_max)
	{}

	MATH_FUNCTION_INLINE void ident_box( box2f & box )
	{
		mt::ident_v2( box.minimum );
		mt::ident_v2( box.maximum );
	}

	MATH_FUNCTION_INLINE void reset( box2f & box, const vec2f &initValue)
	{
		box.maximum = initValue;
		box.minimum = initValue;
	}

	MATH_FUNCTION_INLINE void reset( box2f & box, float x, float y)
	{
		reset(box,vec2f(x,y));
	}

	MATH_FUNCTION_INLINE void add_internal_point( box2f & box, float x,float y)
	{
		if (x>box.maximum.x) box.maximum.x = x;
		if (y>box.maximum.y) box.maximum.y = y;
		if (x<box.minimum.x) box.minimum.x = x;
		if (y<box.minimum.y) box.minimum.y = y;
	}

	MATH_FUNCTION_INLINE void add_internal_point( box2f & box, const vec2f &p)
	{
		add_internal_point(box,p.x, p.y);
	}

	MATH_FUNCTION_INLINE void merge_box( box2f & box, const box2f & external_box )
	{
		if ( external_box.maximum.x > box.maximum.x ) box.maximum.x = external_box.maximum.x;
		if ( external_box.maximum.y > box.maximum.y ) box.maximum.y = external_box.maximum.y;
		if ( external_box.minimum.x < box.minimum.x ) box.minimum.x = external_box.minimum.x;
		if ( external_box.minimum.y < box.minimum.y ) box.minimum.y = external_box.minimum.y;
	}

	MATH_FUNCTION_INLINE void set_box_from_min_max( box2f & box, const vec2f &minimum, const vec2f &maximum)
	{
		box.minimum = minimum;
		box.maximum = maximum;
	}

	MATH_FUNCTION_INLINE void set_box_from_center_and_extent( box2f & box, const vec2f &center, const vec2f &extent)
	{
		box.minimum = center - extent;
		box.maximum = center + extent;
	}

	MATH_FUNCTION_INLINE void set_box_from_oriented_extent( box2f & box, const mt::vec2f & _offset, const mt::vec2f& _size, const mt::mat3f& _wm )
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

		mt::mul_v2_m3( box.minimum, bounds[0], _wm );

		box.maximum = box.minimum;

		mt::vec2f temp;
			
		for( unsigned int i = 1; i != 4; ++i)
		{
			mt::mul_v2_m3(temp, bounds[i], _wm );

			if ( box.maximum.x < temp.x ) box.maximum.x = temp.x;
			if ( box.maximum.y < temp.y ) box.maximum.y = temp.y;

			if ( box.minimum.x > temp.x ) box.minimum.x = temp.x;
			if ( box.minimum.y > temp.y ) box.minimum.y = temp.y;
		}
	}

	MATH_FUNCTION_INLINE bool is_intersect( const vec2f & _aminimum, const vec2f & _amaximum, const vec2f & _bminimum, const vec2f & _bmaximum )
	{
        if( _amaximum.x < _bminimum.x || _aminimum.x > _bmaximum.x )
        {
            return false;
        }

        if( _amaximum.y < _bminimum.y || _aminimum.y > _bmaximum.y )
        {
            return false;
        }
    
        return true;
	}

	MATH_FUNCTION_INLINE bool is_exist( const vec2f & _aminimum, const vec2f & _amaximum, const vec2f & _bminimum, const vec2f & _bmaximum )
	{
		return (_aminimum.x >= _bminimum.x && _amaximum.x <= _bmaximum.x) 
			&& (_aminimum.y >= _bminimum.y && _amaximum.y <= _bmaximum.y);
	}

	MATH_FUNCTION_INLINE bool is_intersect( const box2f & _a, const box2f & _b )
	{
		return is_intersect( _a.minimum, _a.maximum, _b.minimum, _b.maximum );
	}

    MATH_FUNCTION_INLINE bool is_intersect(const box2f & _a, const vec2f & _b)
    {
        return is_intersect( _a.minimum, _a.maximum, _b, _b );
    }

	MATH_FUNCTION_INLINE vec2f box_size( const box2f& _box )
	{
		return _box.maximum - _box.minimum;
	}
}
