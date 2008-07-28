#	pragma once

/*	
	Polygon class.
	-	determine location of a point in convex/concave (O(nlogn), O(n)) polygon.
	-	test if polygon	convex.
*/
#	include "config.h"

#	include "mat3.h"

#	include	"line.h"

#	include <vector>
#	include <algorithm>

namespace mt
{
	typedef std::vector<vec2f> TVectorPoints;

	struct polygon
	{
		MATH_INLINE polygon();
		MATH_INLINE polygon( std::size_t n );
		MATH_INLINE polygon( const polygon & _rhs );

		MATH_INLINE void resize_points( std::size_t _size );
		MATH_INLINE void clear_points();

		MATH_INLINE void add_point(const vec2f& v);
		MATH_INLINE void insert( std::size_t after, const vec2f & v );

		MATH_INLINE std::size_t num_points() const;

		MATH_INLINE polygon & operator = (const polygon& _rhs);

		MATH_INLINE const vec2f & operator [] ( std::size_t i ) const;
		MATH_INLINE vec2f & operator [] ( std::size_t i );

		MATH_INLINE vec2f & front();
		MATH_INLINE const vec2f & front()const;

		MATH_INLINE vec2f & back();
		MATH_INLINE const vec2f & back() const;

		MATH_INLINE const TVectorPoints & get_points() const;

		//	return extrem point in direction
		MATH_INLINE const mt::vec2f & support( const mt::vec2f& normal )  const;

	private:		
		TVectorPoints m_points;
	};

	MATH_INLINE bool cmp_poly_poly(const polygon& _a, const polygon& _b);
	MATH_INLINE bool is_point_inside_convex_polygon(const polygon& poly, const vec2f& p);
	MATH_INLINE bool is_point_inside_polygon(const polygon& poly, const vec2f& p);
	MATH_INLINE bool is_convex_polygon(const polygon& poly);
	MATH_INLINE float orient_polygon(const polygon& poly);

	MATH_INLINE bool is_point_inside_polygon( const polygon& poly, const vec2f& _p, const mt::vec2f& _position, const mt::vec2f& _direction );
	MATH_INLINE bool is_point_inside_polygon( const polygon& poly, const vec2f& _p, const mt::mat3f& wm );

	MATH_INLINE bool intersect_poly_poly( const polygon& _a, const polygon& _b,
		const mt::vec2f & _dirA, const mt::vec2f & _posA,
		const mt::vec2f & _dirB, const mt::vec2f & _posB );

	MATH_INLINE bool intersect_poly_poly( 
		const polygon& _a, const polygon& _b, 
		const mt::mat3f & worldMatrixA,
		const mt::mat3f & worldMatrixB);

	MATH_INLINE bool make_countour_polygon( const TVectorPoints & _polygon, float _width, TVectorPoints & _contour );

	MATH_INLINE bool operator==(const polygon& _a, const polygon& _b);
	MATH_INLINE bool operator!=(const polygon& _a, const polygon& _b);
	MATH_INLINE float polygon_area( const TVectorPoints & _contour );
	int decompose_concave(TVectorPoints & points, std::vector<mt::polygon> & results);
};

#	if MATH_FORCE_INLINE == 1
#	include "polygon_inline.h"
#	endif