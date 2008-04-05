#	include	"polygon.h"
#	include	"simplex.h"
#	include <assert.h>

namespace mt
{
	MATH_INLINE polygon::polygon()
		: convex_value(0)
		, convex_state(false)
	{}

	MATH_INLINE polygon::polygon( TVectorPoints::size_type n)
		: convex_value(0)
		, convex_state(false)
	{
		points.resize(n);
	}

	MATH_INLINE polygon::polygon( const polygon & _rhs )
		: convex_value(0)
		, convex_state(false)
	{
		points = _rhs.points;
	}

	MATH_INLINE polygon & polygon::operator = ( const polygon & _rhs )
	{
		points = _rhs.points;
		return	*this;
	}

	MATH_INLINE mt::vec2f polygon::support( const mt::vec2f & normal ) const
	{
		float max = dot_v2_v2( points[0], normal );
		TVectorPoints::size_type index = 0;

		for( TVectorPoints::size_type it = 1, it_end = points.size(); it != it_end ; ++it )
		{
			float dot = dot_v2_v2( points[it], normal );
			if( dot > max )
			{
				max = dot;
				index = it;
			}
		}

		return points[index];
	}

	MATH_INLINE bool intersect_poly_poly( 
		const polygon& _a, const polygon& _b, 
		const mt::vec2f & _dirA, const mt::vec2f & _posA,
		const mt::vec2f & _dirB, const mt::vec2f & _posB )
	{	
		if( _a.num_points() == 0 || _b.num_points() == 0 )
		{
			return false;
		}

		mt::mat3f worldMatrixA;
		worldMatrixA.v0 = mt::vec3f(_dirA,1);
		worldMatrixA.v1 = mt::vec3f(mt::perp(_dirA),1);
		worldMatrixA.v2 = mt::vec3f(_posA,1);

		mt::mat3f worldMatrixB;
		worldMatrixB.v0 = mt::vec3f(_dirB,1);
		worldMatrixB.v1 = mt::vec3f(mt::perp(_dirB),1);
		worldMatrixB.v2 = mt::vec3f(_posB,1);

		polygon polyA;

		for( polygon::TVectorPoints::size_type it = 0, it_end = _a.num_points(); it != it_end ; ++it )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, _a[ it ], worldMatrixA );
			polyA.add_point(point);
		}

		polygon polyB;

		for( polygon::TVectorPoints::size_type it = 0, it_end = _b.num_points(); it != it_end ; ++it )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, _b[ it ], worldMatrixB );
			polyB.add_point(point);
		}

		// GJK algo for intersection
		simplex_solver solver;
		solver.reset();

		int iteration = 0;

		const int MaxIterations = 100;

		mt::vec3f V(1,0,0);

		mt::vec3f P(polyA.support( V.v2 ),0);
		mt::vec3f Q(polyB.support( -V.v2 ),0);

		mt::vec3f d = P - Q;

		solver.addWPQ( d, P, Q );

		V = -d;

		for( int i = 0; i < MaxIterations; i++ )
		{
			mt::vec3f P(polyA.support( V.v2 ),0);
			mt::vec3f Q(polyB.support( -V.v2 ),0);

			mt::vec3f W = P - Q;

			if ( mt::dot_v3_v3(W, V) > 0 )
			{
				solver.addWPQ( W,P,Q );

				if( solver.update( V ) ) 
				{
					return true;
				}
			}
			else
			{
				return false;
			}

			iteration++;
		}

		return true;
	}

	MATH_INLINE bool intersect_poly_poly( 
		const polygon& _a, const polygon& _b, 
		const mt::mat3f & worldMatrixA,
		const mt::mat3f & worldMatrixB)
	{	
		if( _a.num_points() == 0 || _b.num_points() == 0 )
		{
			return false;
		}

		polygon polyA;

		for ( polygon::TVectorPoints::size_type it = 0, it_end = _a.num_points(); it != it_end; ++it )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, _a[ it ], worldMatrixA );
			polyA.add_point(point);
		}

		polygon polyB;

		for ( polygon::TVectorPoints::size_type it = 0, it_end = _b.num_points(); it != it_end ; ++it )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, _b[ it ], worldMatrixB );
			polyB.add_point(point);
		}

		// GJK algo for intersection
		simplex_solver solver;
		solver.reset();

		int iteration = 0;

		const int MaxIterations = 100;

		mt::vec3f V(1,0,0);

		mt::vec3f P(polyA.support( V.v2 ),0);
		mt::vec3f Q(polyB.support( -V.v2 ),0);

		mt::vec3f d = P - Q;

		solver.addWPQ( d, P, Q );

		V = -d;

		for( int i = 0; i < MaxIterations; i++ )
		{
			mt::vec3f P(polyA.support( V.v2 ),0);
			mt::vec3f Q(polyB.support( -V.v2 ),0);

			mt::vec3f W = P - Q;

			if ( mt::dot_v3_v3(W, V) > 0 )
			{
				solver.addWPQ( W,P,Q );

				if( solver.update( V ) ) 
				{
					return true;
				}
			}
			else
			{
				return false;
			}

			iteration++;
		}

		return true;
	}

	MATH_INLINE void polygon::clear_points()
	{
		points.clear();
	}

	MATH_INLINE void polygon::add_point(const vec2f& v)
	{
		points.push_back(v);

		if( points.size() > 2) 
		{
			check_edges_convex(points.size() - 3);
		}
	}

	MATH_INLINE void polygon::check_edges_convex( size_t i )
	{
		TVectorPoints::size_type size = points.size();
		TVectorPoints::size_type j = (i + 1) % size;
		TVectorPoints::size_type k = (i + 2) % size;

		float value  = pseudo_cross_v2(points[j] - points[i], points[k] - points[j]);

		if (value < 0)
		{
			convex_value |= 1;
		}
		else if (value > 0) 
		{
			convex_value |= 2;
		}

		if (convex_value == 3)
		{
			convex_state = false;
		}

		convex_state = (convex_value != 0);
	}

	MATH_INLINE void polygon::insert( TVectorPoints::size_type after, const vec2f & v )
	{
		TVectorPoints::size_type size = points.size();

		if(after < size)
		{
			points.insert(points.begin() + after, v);

			if( points.size() > 2) 
			{
				check_edges_convex(after - 1);
			}

			if( after + 2 < points.size() ) 
			{
				check_edges_convex(after);
			}
		}
		else
		{
			points.push_back(v);
		}
	}

	MATH_INLINE bool polygon::is_convex() const
	{
		return convex_state;
	}

	MATH_INLINE polygon::TVectorPoints::size_type polygon::num_points() const
	{
		return points.size();
	}

	MATH_INLINE const vec2f& polygon::operator[]( TVectorPoints::size_type i) const
	{
		return points[i];
	}

	MATH_INLINE vec2f& polygon::operator[]( TVectorPoints::size_type i)
	{
		return points[i];
	}

	MATH_INLINE vec2f & polygon::front()
	{
		return points.front();
	}

	MATH_INLINE const vec2f & polygon::front() const
	{
		return points.front();
	}

	MATH_INLINE vec2f & polygon::back()
	{
		return points.back();
	}
	
	MATH_INLINE const vec2f & polygon::back() const
	{
		return points.back();
	}

	MATH_INLINE bool cmp_poly_poly(const polygon& _a, const polygon& _b)
	{
		polygon::TVectorPoints::size_type size_a = _a.num_points();
		polygon::TVectorPoints::size_type size_b = _b.num_points();
		
		if( size_a != size_b )
		{
			return	false;
		}

		if( size_a == 0 )
		{
			return	true;
		}

		return std::equal(
			&_a.front(),
			&_a.back(), 
			stdext::checked_array_iterator<const vec2f *>(&_b.front(), size_b) 
			);
	}

	MATH_INLINE bool operator==(const polygon& _a, const polygon& _b)
	{
		return cmp_poly_poly(_a,_b);
	}

	MATH_INLINE bool operator!=(const polygon& _a, const polygon& _b)
	{
		return !operator==(_a,_b);
	}

	/*	
		Test if point p lies inside convex polygon given by polygon vertices.
		In debug mode test if polygon convex.
		(c)	Kesha
	*/
	MATH_INLINE bool is_point_inside_convex_polygon(const polygon& poly, const vec2f& p)
	{
#ifdef	_DEBUG
		assert(is_convex_polygon(poly));
#endif

		polygon::TVectorPoints::size_type size = poly.num_points();

		if( size == 0 )
		{
			return false;
		}

		polygon::TVectorPoints::size_type L = 0;
		polygon::TVectorPoints::size_type H = size;

		do
		{
			polygon::TVectorPoints::size_type M = (L + H) / 2;

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

		return is_left_v2(poly[L], poly[H], p) > 0;
	}

	/*	
		Test if point p lies inside polygon given by polygon vertices.
		(c)	IROV
	*/
	MATH_INLINE bool is_point_inside_polygon(const polygon& poly, const vec2f& p)
	{
		polygon::TVectorPoints::size_type size = poly.num_points();

		if(size == 0)
		{
			return false;
		}

		polygon::TVectorPoints::size_type intersect_counter = 0;

		vec2f prev = poly[size-1];
		
		for (polygon::TVectorPoints::size_type i = 0; i < size; ++i)
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

	MATH_INLINE bool is_point_inside_polygon( const polygon& poly, const vec2f& _p, const mt::mat3f& wm )
	{
		polygon::TVectorPoints::size_type size = poly.num_points();

		if( size == 0 )
		{
			return false;
		}

		polygon::TVectorPoints::size_type intersect_counter = 0;

		mt::vec2f prev;
		mt::mul_v2_m3( prev, poly[ size - 1], wm );

		for( polygon::TVectorPoints::size_type i = 0; i < size; ++i )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, poly[ i ], wm );

			if( ( point.y > _p.y) ^ (prev.y > _p.y) )
			{
				if( prev.x + (_p.y - prev.y) / (point.y - prev.y) * (point.x - prev.x) > _p.x )
				{
					++intersect_counter;
				}
			}
			prev = point;
		}

		return intersect_counter & 1;
	}

	MATH_INLINE bool is_point_inside_polygon( const polygon& poly, const vec2f& _p, const mt::vec2f& _position, const mt::vec2f& _direction )
	{
		polygon::TVectorPoints::size_type size = poly.num_points();

		mt::mat3f wm;
		wm.v0 = mt::vec3f(_direction,1);
		wm.v1 = mt::vec3f(mt::perp(_direction),1);
		wm.v2 = mt::vec3f(_position,1);

		if( size == 0 )
		{
			return false;
		}

		polygon::TVectorPoints::size_type intersect_counter = 0;

		mt::vec2f prev;
		mt::mul_v2_m3( prev, poly[ size - 1], wm );

		for ( polygon::TVectorPoints::size_type i = 0; i < size; ++i )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, poly[ i ], wm );

			if (( point.y > _p.y) ^ (prev.y > _p.y))
			{
				if (prev.x + (_p.y - prev.y) / (point.y - prev.y) * (point.x - prev.x) > _p.x)
				{
					++intersect_counter;
				}
			}
			prev = point;
		}

		return intersect_counter & 1;
	}

	/*	
		Test if polygon is convex.  
	*/
	MATH_INLINE bool is_convex_polygon(const polygon& poly)
	{
		polygon::TVectorPoints::size_type size = poly.num_points();

		if (size < 3)
		{
			return false;
		}

		char flag = 0;

		for (polygon::TVectorPoints::size_type i = 0; i < size; i++) 
		{
			polygon::TVectorPoints::size_type j = (i + 1) % size;
			polygon::TVectorPoints::size_type k = (i + 2) % size;

			float value = pseudo_cross_v2(poly[j] - poly[i], poly[k] - poly[j]);

			if (value < 0)
			{
				flag |= 1;
			}
			else if (value > 0) 
			{
				flag |= 2;
			}

			if (flag == 3)
			{
				return false;
			}
		}

		return flag != 0;
	}

	/*
		>0 CCW 
	    =0 (degenerate)
	    <0 CW
	*/
	MATH_INLINE float orient_polygon(const polygon& poly)
	{
		polygon::TVectorPoints::size_type rmin = 0;
		float xmin = poly[0].x;
		float ymin = poly[0].y;

		polygon::TVectorPoints::size_type size = poly.num_points();

		for (polygon::TVectorPoints::size_type i = 1; i < size; i++)
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