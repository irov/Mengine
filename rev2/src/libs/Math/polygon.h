/*	
	Polygon class.
	-	determine location of a point in convex/concave (O(nlogn), O(n)) polygon.
	-	test if polygon	convex.
*/
#pragma once

#include <vector>
#include <algorithm>
#include "vec2.h"
#include "mat3.h"



namespace mt
{
	struct polygon
	{
		polygon();
		polygon(size_t n);
		polygon(const polygon&	_rhs);

		void	clear_points();

		void	add_point(const vec2f& v);
		void	insert(size_t after, const vec2f& v);

		size_t	num_points() const;

		polygon&	operator=(const polygon& _rhs);

		const vec2f&	operator[](size_t i) const;
		vec2f&			operator[](size_t i);

		vec2f & front();
		const vec2f & front()const;

		vec2f & back();
		const vec2f & back() const;

		bool	is_convex() const;

		//	return extrem point in direction
		mt::vec2f	support( const mt::vec2f& normal )  const;

	private:
		std::vector<vec2f> points;

		char convex_value;
		bool convex_state;
		void check_edges_convex(size_t i);
	};

	bool	cmp_poly_poly(const polygon& _a, const polygon& _b);
	bool	is_point_inside_convex_polygon(const polygon& poly, const vec2f& p);
	bool	is_point_inside_polygon(const polygon& poly, const vec2f& p);
	bool	is_convex_polygon(const polygon& poly);
	float	orient_polygon(const polygon& poly);

	bool	is_point_inside_polygon( const polygon& poly, const vec2f& _p, const mt::vec2f& _position, const mt::vec2f& _direction );
	bool	is_point_inside_polygon( const polygon& poly, const vec2f& _p, const mt::mat3f& wm );

	bool	intersect_poly_poly( const polygon& _a, const polygon& _b,
		const mt::vec2f & _dirA, const mt::vec2f & _posA,
		const mt::vec2f & _dirB, const mt::vec2f & _posB );

	bool	intersect_poly_poly( 
		const polygon& _a, const polygon& _b, 
		const mt::mat3f & worldMatrixA,
		const mt::mat3f & worldMatrixB);

	bool	operator==(const polygon& _a, const polygon& _b);
	bool	operator!=(const polygon& _a, const polygon& _b);
};
