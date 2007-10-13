#	include	"simplex.h"

namespace	mt
{
	simplex_solver::simplex_solver()
		: size(0)
	{}

	void	simplex_solver::reset()
	{
		size = 0;
	};

	void	simplex_solver::removeFromSimplexUnusedPoints(ABC abc)
	{
		if( (size >= 3) && (abc.C == false) )
		{
			size--;
			W[2] = W[size];
			P[2] = P[size];
			Q[2] = Q[size];
		}

		if( (size >= 2) && (abc.B == false) )
		{
			size--;
			W[1] = W[size];
			P[1] = P[size];
			Q[1] = Q[size];
		}

		if( (size >= 1) && (abc.A == false) )
		{
			size--;
			W[0] = W[size];
			P[0] = P[size];
			Q[0] = Q[size];
		}

	}

	bool	simplex_solver::testSimplex( const mt::vec3f& _W)
	{
		for( size_t i = 0; i < size; i++ )
		{
			if (W[i].x == _W.x && W[i].y == _W.y && W[i].z == _W.z)
			{
				return true;
			}
		}
		return false;
	}

	void	simplex_solver::addVertex( const mt::vec3f& _W, const mt::vec3f& _P, const mt::vec3f& _Q )
	{
		W[size] = _W;
		P[size] = _P;
		Q[size++] = _Q;
	}
	
	void	simplex_solver::update( mt::vec3f & V )
	{
		ABC Usage;

		Usage.reset();

		if(size == 0)
		{
			assert(0);
		}
		else
		if(size == 1)
		{
			V = W[0];
			ClosestPoints[0] = P[0];
			ClosestPoints[1] = Q[0];
		}
		else
		if(size == 2)
		{
			mt::vec3f	origin(0, 0, 0);
			mt::vec3f	ab = W[1] - W[0];
			
			float t = mt::dot_v3_v3(origin - W[0], ab) / ( mt::dot_v3_v3(ab, ab) );
			
			if  ( t >= 0.0f && t <= 1.0f )	
			{
				Usage.A = true;
				Usage.B = true;
			}

			if ( t < 0.0f )
			{
				t = 0.0f;
				Usage.A = true;
			}

			if ( t > 1.0f )	
			{
				t = 1.0f;
				Usage.B = true;
			}
			
			ClosestPoints[0] = P[0] + t * (P[1] - P[0]);
			ClosestPoints[1] = Q[0] + t * (Q[1] - Q[0]);

			V = ClosestPoints[0] - ClosestPoints[1]; 

			removeFromSimplexUnusedPoints(Usage);
		}
		else
		if(size == 3)
		{
			mt::vec3f	origin(0, 0, 0);
			float u = 0.0f;
			float v = 0.0f;
			float w = 0.0f;

			mt::vec3f	ab = W[1] - W[0];
			mt::vec3f	ac = W[2] - W[0];	
			mt::vec3f	ap = origin - W[0];	

			float d1 = mt::dot_v3_v3(ab, ap);
			float d2 = mt::dot_v3_v3(ac, ap);

			for(;;)
			{
				if( d1 <= 0.0f && d2 <= 0.0f ) 
				{
					u = 1;
					v = 0;
					w = 0;
					Usage.A = true;
					break;
				}

				mt::vec3f bp = origin - W[1];
				float d3 = mt::dot_v3_v3(ab, bp);
				float d4 = mt::dot_v3_v3(ac, bp);

				if( d3 >= 0.0f && d4 <= d3 ) 
				{
					u = 0;
					v = 1;
					w = 0;
					Usage.B = true;
					break;
				}

				float vc = d1*d4-d3*d2;
				if(vc <= 0.0f && d1>= 0.0f && d3 <= 0.0f)
				{
					float v = d1/(d1-d3);
					u = 1-v;
					v = v;
					w = 0;
					Usage.A = true;
					Usage.B = true;
					break;
				}

				mt::vec3f cp = origin - W[2];
				float d5 = mt::dot_v3_v3(ab, cp);
				float d6 = mt::dot_v3_v3(ac, cp);

				if( d6 >= 0.0f && d5 <= d6 ) 
				{
					u = 0;
					v = 0;
					w = 1;
					Usage.C = true;
					break;
				}

				float vb = d5*d2-d1*d6;
				if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
				{
					float w_ = d2/(d2-d6);
					u = 1-w_;
					v = 0;
					w = w_;
					Usage.A = true;
					Usage.C = true;
					break;
				}

				float va = d3*d6-d5*d4;

				if(va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
				{
					float w_ = (d4 - d3)/((d4 - d3)+(d5 - d6));
					u = 0;
					v = 1-w_;
					w = w_;
					Usage.B = true;
					Usage.C = true;
					break;
				}

				float denom = 1.0f / (va + vb + vc);

				v = vb * denom;
				w = vc * denom;

				u = 1 - v - w;

				Usage.B = true;
				Usage.C = true;
				break;
			}

			ClosestPoints[0] = u * P[0] + v * P[1] + w * P[2];
			ClosestPoints[1] = u * Q[0] + v * Q[1] + w * Q[2];

			V = ClosestPoints[0] - ClosestPoints[1]; 

			removeFromSimplexUnusedPoints(Usage);
		}
	}
};