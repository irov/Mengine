#	pragma once

#	include	"vec3.h"

#	include "config.h"

namespace mt
{
	class simplex_solver
	{
	public:
		MATH_INLINE simplex_solver();

	public:
		MATH_INLINE void reset();
		MATH_INLINE void addWPQ( const mt::vec3f & w, const mt::vec3f & p, const mt::vec3f & q  );
		MATH_INLINE bool update( mt::vec3f & V );

	private:
		MATH_INLINE void remove( int _size );
		MATH_INLINE bool solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f & V );
		MATH_INLINE bool solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f AC, mt::vec3f & V );

	private:
		int size;
		float eps;

		mt::vec3f W[3];
		mt::vec3f P[3];
		mt::vec3f Q[3];
	};
};