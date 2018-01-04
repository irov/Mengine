#	pragma once

#	include "config.h"

#	include "vec2.h"

#   include <stdint.h>

namespace mt
{    
	class convex8
	{
    public:
        MENGINE_MATH_METHOD_INLINE convex8();

    public:
        MENGINE_MATH_METHOD_INLINE const mt::vec2f & operator [] ( size_t i ) const;
        MENGINE_MATH_METHOD_INLINE mt::vec2f & operator [] ( size_t i );

    public:
        MENGINE_MATH_METHOD_INLINE void add( const mt::vec2f & v );
        MENGINE_MATH_METHOD_INLINE const mt::vec2f & front() const;
        MENGINE_MATH_METHOD_INLINE const mt::vec2f & back() const;
        MENGINE_MATH_METHOD_INLINE uint32_t size() const;
        MENGINE_MATH_METHOD_INLINE bool empty() const;
        MENGINE_MATH_METHOD_INLINE void clear();

    protected:
		mt::vec2f point[8];
		uint32_t count;
	};

	MENGINE_MATH_FUNCTION_INLINE void convex8_intersect( const mt::convex8 & _convex, const mt::convex8 & _clip, mt::convex8 & _result );
    MENGINE_MATH_FUNCTION_INLINE float convex8_area( const mt::convex8 & _convex );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#	    include "convex8_inline.h"
#	endif
