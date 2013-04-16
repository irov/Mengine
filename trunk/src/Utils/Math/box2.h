#	pragma once

#	include "config.h"

#	include "mat3.h"

namespace mt
{
	struct box2f
	{
		/*union
		{
			struct {vec2f vb,ve;};
			struct {vec2f v[2];};
			struct {vec2f min,max;};
		};*/

		vec2f minimum; 
		vec2f maximum;

		MATH_METHOD_INLINE box2f();
		MATH_METHOD_INLINE box2f( const box2f & _box );
		MATH_METHOD_INLINE box2f( const vec2f & _vb, const vec2f & _ve );
	};

	MATH_FUNCTION_INLINE void reset(box2f & box,const vec2f & initValue);
	MATH_FUNCTION_INLINE void reset(box2f & box,float x, float y);
	MATH_FUNCTION_INLINE void add_internal_point(box2f & box, float x,float y);
	MATH_FUNCTION_INLINE void add_internal_point(box2f & box, const vec2f & p);
	MATH_FUNCTION_INLINE void merge_box( box2f & box, const box2f & external_box );
	MATH_FUNCTION_INLINE void set_box_from_min_max(box2f & box, const vec2f & min, const vec2f & max);
	MATH_FUNCTION_INLINE void set_box_from_center_and_extent(box2f & box, const vec2f & center, const vec2f & extent);
	MATH_FUNCTION_INLINE void set_box_from_oriented_extent( box2f & box, const mt::vec2f & _offset, const mt::vec2f & _size, const mt::mat3f & _wm );
    MATH_FUNCTION_INLINE bool is_exist( const vec2f & _aminimum, const vec2f & _amaximum, const vec2f & _bminimum, const vec2f & _bmaximum );
	MATH_FUNCTION_INLINE bool is_intersect(const box2f & _a, const box2f & _b);
    MATH_FUNCTION_INLINE bool is_intersect(const box2f & _a, const vec2f & _b);
	MATH_FUNCTION_INLINE vec2f box_size( const box2f& _box );
}

#	if MATH_FORCE_INLINE == 1
#	include "box2_inline.h"
#	endif
