#	include "utils.h"

#	include <math.h>

namespace mt
{
	MENGINE_MATH_METHOD_INLINE simplex_solver::simplex_solver()
		: size(0)
		, eps(0.00001f)
	{}

	MENGINE_MATH_METHOD_INLINE void simplex_solver::reset()
	{
		size = 0;
	}

	MENGINE_MATH_METHOD_INLINE void simplex_solver::addWPQ( const mt::vec3f & w, const mt::vec3f & p, const mt::vec3f & q  )
	{
		W[size] = w;
		P[size] = p;
		Q[size++] = q;
	}

	MENGINE_MATH_METHOD_INLINE bool simplex_solver::update( mt::vec3f & V )
	{
		bool found = false; 

		if( size == 2 )
		{
			found = solve( -W[1], W[0] - W[1], V );
		}
		else if( size == 3 )
		{
			found = solve( -W[2], W[1] - W[2], W[0] - W[2], V );
		}

		return found;
	}

	MENGINE_MATH_METHOD_INLINE void simplex_solver::remove_1()
	{
		W[0] = W[1];
		P[0] = P[1];
		Q[0] = Q[1];

		size = 1;
	}

	MENGINE_MATH_METHOD_INLINE void simplex_solver::remove_2()
	{
		W[1] = W[2];
		P[1] = P[2];
		Q[1] = Q[2];

		size = 2;
	}

	MENGINE_MATH_METHOD_INLINE bool simplex_solver::solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f & V )
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
			remove_1();
		}

		return false;
	}

	MENGINE_MATH_METHOD_INLINE bool simplex_solver::solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f AC, mt::vec3f & V )
	{
		mt::vec3f ABC = mt::cross_v3_v3( AB, AC );

		mt::vec3f ABCxAB = mt::cross_v3_v3( ABC, AB );
		mt::vec3f ABCxAC = mt::cross_v3_v3( ABC, AC );
		
		float AB_SIDE = mt::dot_v3_v3( ABCxAB, AO );
		float AC_SIDE = mt::dot_v3_v3( ABCxAC, AO );

		if( AB_SIDE < -eps )
		{
			remove_1();
			remove_2();
			return solve(AO,AB,V);
		}
		else if( AC_SIDE > eps )
		{
			remove_2();
			return solve(AO,AC,V);
		}
		else
		{
			float d = mt::dot_v3_v3(ABC,AO);

			if( mt::cmp_f_z( d ) == false )
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
