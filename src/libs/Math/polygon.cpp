#	include	"polygon.h"
#	include	"simplex.h"

namespace mt
{
	polygon::polygon()
		: convex_value(0)
		, convex_state(false)
	{}

	polygon::polygon(size_t n)
		: convex_value(0)
		, convex_state(false)
	{
		points.resize(n);
	}

	polygon::polygon(const polygon&	_rhs)
		: convex_value(0)
		, convex_state(false)
	{
		points = _rhs.points;
	}

	polygon&	polygon::operator=(const polygon& _rhs)
	{
		points = _rhs.points;
		return	*this;
	}

	mt::vec2f	polygon::support( const mt::vec2f& normal ) const
	{
		float max = dot_v2_v2( points[0], normal );
		int index = 0;

		for( int i = 1; i < points.size(); i++ )
		{
			float dot = dot_v2_v2( points[i], normal );
			if( dot > max )
			{
				max = dot;
				index = i;
			}
		}

		return points[index];
	}

	bool	intersect_poly_poly( const polygon& _a, const polygon& _b )
	{
		float nu = 0;
		float delta = 0;
		bool close_enough = false;
		float eps = 0.0001f;

		simplex_solver solver;
		
		int iteration = 0;

		mt::vec3f	V(1,0,0);

		while(!close_enough)
		{
			mt::vec3f P = mt::vec3f( _a.support( -V.v2 ), 0);
			mt::vec3f Q = mt::vec3f( _b.support( V.v2 ), 0);
			mt::vec3f W = P - Q;

			float v = V.length();

			delta = dot_v3_v3(V,W) / v;

			if(delta > 0) 
			{
				return false;
			}

			nu = std::max(nu, delta);

			close_enough = fabs(v - nu) < eps;

			if(!close_enough)
			{
				if( solver.testSimplex(W) == true )
				{
					return false;
				}
				solver.addVertex( W, P, Q );
				solver.update( V );
			}

			iteration++;

			if(iteration > 300)
			{
				return false;
			}
		}

		return true;
	}

	bool	closest_point_poly_poly( const polygon& _a, const polygon& _b, mt::vec2f & _p, mt::vec2f & _q )
	{
		float nu = 0;
		float delta = 0;
		bool close_enough = false;
		float eps = 0.0001f;

		simplex_solver solver;
		
		int iteration = 0;

		bool intersect = true;

		mt::vec3f	V(1,0,0);

		while(!close_enough)
		{
			mt::vec3f P = mt::vec3f( _a.support( -V.v2 ), 0);
			mt::vec3f Q = mt::vec3f( _b.support( V.v2 ), 0);
			mt::vec3f W = P - Q;

			float v = V.length();

			delta = dot_v3_v3(V,W) / v;

			if(delta > 0) 
			{
				intersect = false;
			}

			nu = std::max(nu, delta);

			close_enough = fabs(v - nu) < eps;

			if(!close_enough)
			{
				if( solver.testSimplex( W ) == true )
				{
					return false;
				}

				solver.addVertex( W, P, Q );
				solver.update( V );
			}

			iteration++;

			if(iteration > 300)
			{
				return false;
			}
		}

		_p = solver.ClosestPoints[0].v2;
		_q = solver.ClosestPoints[1].v2;

		return intersect;
	}


	void polygon::clear_points()
	{
		points.clear();
	}

	void polygon::add_point(const vec2f& v)
	{
		points.push_back(v);

		if( points.size() > 2) 
			check_edges_convex(points.size() - 3);
	}

	void polygon::check_edges_convex( size_t i )
	{
		size_t j = (i + 1) % points.size();
		size_t k = (i + 2) % points.size();

		float value  = pseudo_cross_v2(points[j] - points[i], points[k] - points[j]);

		if (value < 0)	convex_value |= 1;
		else if (value > 0) convex_value |= 2;

		if (convex_value == 3)	convex_state = false;
		convex_state = (convex_value != 0);
	}

	void polygon::insert(size_t after, const vec2f& v)
	{
		size_t size = points.size();

		if(after < size)
		{
			points.insert(points.begin() + after, v);

			if( points.size() > 2) 
				check_edges_convex(after - 1);

			if( after + 2 < points.size()) 
				check_edges_convex(after);
		}
		else
		{
			points.push_back(v);
		}
	}

	bool	polygon::is_convex() const
	{
		return convex_state;
	}

	size_t	polygon::num_points() const
	{
		return	points.size();
	}

	const vec2f& polygon::operator[](size_t i) const
	{
		return points[i];
	}

	vec2f& polygon::operator[](size_t i)
	{
		return points[i];
	}

	vec2f & polygon::front()
	{
		return points.front();
	}

	const vec2f & polygon::front()const
	{
		return points.front();
	}

	vec2f & polygon::back()
	{
		return points.back();
	}
	
	const vec2f & polygon::back()const
	{
		return points.back();
	}

	bool	cmp_poly_poly(const polygon& _a, const polygon& _b)
	{
		size_t size_a = _a.num_points();
		size_t size_b = _b.num_points();
		if(size_a!=size_b)	return	false;
		if(!size_a) return	true;
		return std::equal(
			&_a.front(),
			&_a.back(), 
			stdext::checked_array_iterator<const vec2f *>(&_b.front(), size_b) 
			);
	}

	bool	operator==(const polygon& _a, const polygon& _b)
	{
		return	cmp_poly_poly(_a,_b);
	}

	bool	operator!=(const polygon& _a, const polygon& _b)
	{
		return	!operator==(_a,_b);
	}

	/*	
		Test if point p lies inside convex polygon given by polygon vertices.
		In debug mode test if polygon convex.
		(c)	Kesha
	*/
	bool is_point_inside_convex_polygon(const polygon& poly, const vec2f& p)
	{
		#ifdef	_DEBUG
			assert(is_convex_polygon(poly));
		#endif

		size_t size = poly.num_points();

		if(size == 0)
		{
			return false;
		}

		size_t L = 0;
		size_t H = size;

		do
		{
			size_t M = (L + H) / 2;

			if (is_left_v2(poly[0], poly[M], p) > 0)
			{
				L = M;
			}
			else
			{
				H = M;
			}
		} 
		while (L + 1 < H);

		if (L == 0 || H == size) 
		{
			return false;
		}

		return 	is_left_v2(poly[L], poly[H], p) > 0;
	}

	/*	
		Test if point p lies inside polygon given by polygon vertices.
		(c)	IROV
	*/
	bool is_point_inside_polygon(const polygon& poly, const vec2f& p)
	{
		size_t size = poly.num_points();

		if(size == 0)
		{
			return false;
		}

		size_t intersect_counter = 0;

		vec2f prev = poly[size-1];
		
		for (size_t i = 0; i < size; ++i)
		{
			if ((poly[i].y > p.y) ^ (prev.y > p.y))
			{
				if (prev.x + (p.y - prev.y) / (poly[i].y - prev.y) * (poly[i].x - prev.x) > p.x)
				{
					++intersect_counter;
				}
			}
			prev = poly[i];
		}
		
		return intersect_counter & 1;
	}
	/*	
		Test if polygon is convex.  
	*/
	bool is_convex_polygon(const polygon& poly)
	{
		size_t size = poly.num_points();

		if (size < 3)
		{
			return false;
		}

		char flag = 0;

		for (size_t i = 0; i < size; i++) 
		{
			size_t j = (i + 1) % size;
			size_t k = (i + 2) % size;

			float value  = pseudo_cross_v2(poly[j] - poly[i], poly[k] - poly[j]);

			if (value < 0)	flag |= 1;
			else if (value > 0) flag |= 2;

			if (flag == 3)	return false;
		}

		return flag != 0;
	}

	/*
		>0 CCW 
	    =0 (degenerate)
	    <0 CW
	*/
	float	orient_polygon(const polygon& poly)
	{
		size_t rmin = 0;
		float xmin = poly[0].x;
		float ymin = poly[0].y;

		size_t size = poly.num_points();

		for (size_t i = 1; i < size; i++)
		{
			if (poly[i].y > ymin)	continue;

			if (poly[i].y == ymin)
			{    
				if (poly[i].x < xmin) 	continue;
			}

			rmin = i;          
			xmin = poly[i].x;
			ymin = poly[i].y;
		}
		return	rmin == 0 ? is_left_v2(poly[size - 1], poly[0], poly[1]) : is_left_v2(poly[rmin - 1], poly[rmin], poly[rmin + 1]);
	}
};