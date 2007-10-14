#	include	"vec3.h"

namespace	mt
{
	class	simplex_solver
	{
	public:
		simplex_solver();
		void	reset();
		void	addWPQ( const mt::vec3f & w, const mt::vec3f & p, const mt::vec3f & q  );
		bool	update( mt::vec3f & V );
	private:
		int size;
		float eps;

		mt::vec3f	W[3];
		mt::vec3f	P[3];
		mt::vec3f	Q[3];

		void	remove( int _size );
		bool	solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f & V );
		bool	solve( mt::vec3f AO, mt::vec3f AB, mt::vec3f AC, mt::vec3f & V );
	};
};