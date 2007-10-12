#	include	"vec3.h"

namespace	mt
{
	class	simplex_solver
	{
	public:
		simplex_solver();

		mt::vec3f	W[3];
		mt::vec3f	P[3];
		mt::vec3f	Q[3];

		mt::vec3f	ClosestPoints[2];	

		void	reset();
		bool	testSimplex( const mt::vec3f& _W );
		void	addVertex( const mt::vec3f& _W, const mt::vec3f& _P, const mt::vec3f& _Q );
		void	update( mt::vec3f & V );

	private:
		size_t size;

		struct	ABC
		{
			bool A;
			bool B;
			bool C;
			void reset()
			{
				A = B = C = false;
			};
		}; 

		void	removeFromSimplexUnusedPoints(ABC abc);
	};
};