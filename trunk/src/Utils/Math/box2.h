#	pragma once

#	include "config.h"

#	include "mat3.h"

namespace mt
{
	struct box2f
	{
		vec2f minimum; 
		vec2f maximum;

		MENGINE_MATH_METHOD_INLINE box2f();
		MENGINE_MATH_METHOD_INLINE box2f( float _minx, float _miny, float _maxx, float _maxy );
		MENGINE_MATH_METHOD_INLINE box2f( const box2f & _box );
		MENGINE_MATH_METHOD_INLINE box2f( const vec2f & _vb, const vec2f & _ve );
	};

	MENGINE_MATH_FUNCTION_INLINE void ident_box(box2f & box);
	MENGINE_MATH_FUNCTION_INLINE void reset(box2f & box,const vec2f & initValue);
	MENGINE_MATH_FUNCTION_INLINE void reset(box2f & box,float x, float y);
	MENGINE_MATH_FUNCTION_INLINE void add_internal_point(box2f & box, float x,float y);
	MENGINE_MATH_FUNCTION_INLINE void add_internal_point(box2f & box, const vec2f & p);
	MENGINE_MATH_FUNCTION_INLINE void merge_box( box2f & box, const box2f & external_box );
	MENGINE_MATH_FUNCTION_INLINE void transpose_box( box2f & _box, const mt::vec2f & _pos );
	MENGINE_MATH_FUNCTION_INLINE void set_box_from_min_max(box2f & box, const vec2f & min, const vec2f & max);
	MENGINE_MATH_FUNCTION_INLINE void set_box_from_center_and_extent(box2f & box, const vec2f & center, const vec2f & extent);
	MENGINE_MATH_FUNCTION_INLINE void set_box_from_oriented_extent( box2f & box, const mt::vec2f & _offset, const mt::vec2f & _size, const mt::mat3f & _wm );
    MENGINE_MATH_FUNCTION_INLINE bool is_exist( const vec2f & _aminimum, const vec2f & _amaximum, const vec2f & _bminimum, const vec2f & _bmaximum );
	MENGINE_MATH_FUNCTION_INLINE bool is_intersect( const vec2f & _aminimum, const vec2f & _amaximum, const vec2f & _bminimum, const vec2f & _bmaximum );
	MENGINE_MATH_FUNCTION_INLINE bool is_intersect(const box2f & _a, const box2f & _b);
    MENGINE_MATH_FUNCTION_INLINE bool is_intersect(const box2f & _a, const vec2f & _b);
	MENGINE_MATH_FUNCTION_INLINE bool is_intersect(const box2f & _a, const vec2f & _b, float _radius);
	MENGINE_MATH_FUNCTION_INLINE vec2f box_size( const box2f& _box );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "box2_inline.h"
#	endif
