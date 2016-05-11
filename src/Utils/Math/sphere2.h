#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{
	class sphere2
	{
	public:
		sphere2()
		{
		}

		sphere2( const mt::vec2f & _position, float _radius )
			: position( _position )
			, radius(_radius)
		{
		}

		sphere2( const mt::sphere2 & _sphere )
			: position( _sphere.position )
			, radius( _sphere.radius )
		{
		}

	public:
		mt::vec2f position;
		float radius;
	};

	MENGINE_MATH_FUNCTION_INLINE bool sphere2_intersect_sphere2( const mt::sphere2 & _sphere0, const mt::sphere2 & _sphere1 );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#		include "sphere2_inline.h"
#	endif
