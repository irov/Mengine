#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	struct rectf
	{
		float x;
		float y;
		float width;
		float height;

		MENGINE_MATH_METHOD_INLINE rectf();
		MENGINE_MATH_METHOD_INLINE rectf(const rectf & _rect);
		MENGINE_MATH_METHOD_INLINE explicit rectf(float _x, float _y, float _width, float _height);
		MENGINE_MATH_METHOD_INLINE rectf & operator = (const rectf & _rhs);

        MENGINE_MATH_METHOD_INLINE bool empty() const;
	};

    MENGINE_MATH_FUNCTION_INLINE bool rect_contains_point( const rectf & _rect, const mt::vec2f & _point );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#		include "rect_inline.h"
#	endif