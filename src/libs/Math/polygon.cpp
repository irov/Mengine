#include	"polygon.h"

namespace mt
{
	polygon::polygon()
	{}

	polygon::polygon(const polygon&	_rhs)
	{
		points = _rhs.points;
	}

	polygon&	polygon::operator=(const polygon& _rhs)
	{
		points = _rhs.points;
		return	*this;
	}

	polygon::polygon(size_t n)
	{
		points.resize(n);
	}

	void polygon::clear_points()
	{
		points.clear();
	}

	void polygon::add_point(const vec2f& v)
	{
		points.push_back(v);
	}

	void polygon::insert(size_t after, const vec2f& v)
	{
		size_t size = points.size();

		if(after < size)
		{
			points.insert(points.begin()+after,v);
		}
		else
		{
			points.push_back(v);
		}
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
			float z  = (poly[j].x - poly[i].x) * (poly[k].y - poly[j].y) - (poly[j].y - poly[i].y) * (poly[k].x - poly[j].x);
			if (z < 0)	flag |= 1;
			else if (z > 0) flag |= 2;
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

		for (size_t i=1; i<size; i++)
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
		return	rmin == 0 ? is_left_v2(poly[size-1], poly[0], poly[1]) : is_left_v2(poly[rmin-1], poly[rmin], poly[rmin+1]);
	}
};