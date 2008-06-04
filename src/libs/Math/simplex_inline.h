#	include <assert.h>

#	include <math.h>

namespace mt
{
	MATH_INLINE simplex_solver::simplex_solver()
		: size(0)
		, eps(0.00001f)
	{}

	MATH_INLINE void simplex_solver::reset()
	{
		size = 0;
	}

	MATH_INLINE void simplex_solver::addWPQ( const mt::vec3f & w, const mt::vec3f & p, const mt::vec3f & q  )
	{
		W[size] = w;
		P[size] = p;
		Q[size++] = q;
	}

	MATH_INLINE bool simplex_solver::update( mt::vec3f & V )
	{
		bool found = false; 

		if( size == 0 )
		{
			assert(0);
		}
		else if( size == 1 )
		{
			assert(0);
		}
		else if( size == 2 )
		{
			found = solve( -W[1], W[0] - W[1], V );
		}
		else if( size == 3 )
		{
			found = solve( -W[2], W[1] - W[2], W[0] - W[2], V );
		}
		else if( size == 4 )
		{
			assert(0);
		}

		return found;
	}

	MATH_INLINE void simplex_solver::remove( int _size )
	{
		assert( _size );

		W[ _size - 1 ] = W[ _size ];
		P[ _size - 1 ] = P[ _size ];
		Q[ _size - 1 ] = Q[ _size ];

		size = _size;
	}

	MATH_INLINE bool simplex_solver::solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f & V )
	{
		if( mt::dot_v3_v3( AO, AB ) > 0 )
		{
			mt::vec3f AC = mt::cross_v3_v3(AB, AO);

			float len = AC.length();

			if( len > eps )
			{
				V = mt::cross_v3_v3(AC, AB);
			}
			else
			{
				return true;
			}
		}
		else
		{
			V = AO;
			remove(1);
		}

		return false;
	}

	MATH_INLINE bool simplex_solver::solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f AC, mt::vec3f & V )
	{
		mt::vec3f ABC = mt::cross_v3_v3( AB, AC );

		mt::vec3f ABCxAB = mt::cross_v3_v3( ABC, AB );
		mt::vec3f ABCxAC = mt::cross_v3_v3( ABC, AC );
		
		float AB_SIDE = mt::dot_v3_v3( ABCxAB, AO );
		float AC_SIDE = mt::dot_v3_v3( ABCxAC, AO );

		if( AB_SIDE < -eps )
		{
			remove(1);
			remove(2);
			return solve(AO,AB,V);
		}
		else if( AC_SIDE > eps )
		{
			remove(2);
			return solve(AO,AC,V);
		}
		else
		{
			float d = mt::dot_v3_v3(ABC,AO);

			if( fabsf(d) > eps )
			{
				V = ( d > 0) ? ABC : -ABC;
				return false;
			} 
			else
			{
				return true;
			}
		}
	}
};