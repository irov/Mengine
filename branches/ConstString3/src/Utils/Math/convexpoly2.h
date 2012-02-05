//#	pragma once
//#	include <vector>
//#	include <list>
//#	include "mat3.h"
//#	include "polygon.h"
//namespace mt
//{
//	class convexpoly2
//	{
//	public:
//		typedef std::vector<vec2f> TVectorPoints;
//
//		MATH_INLINE convexpoly2();
//		MATH_INLINE convexpoly2(const std::vector<mt::vec2f> & _points);
//		MATH_INLINE void set_transformation(const mt::vec2f& _dirA,const mt::vec2f& _posA);
//
//		MATH_INLINE mt::vec2f get_transformed_vertex(size_t i) const;
//		MATH_INLINE mt::vec2f get_transformed_edge(size_t i) const;
//
//		MATH_INLINE size_t num_points() const;
//		MATH_INLINE void add_point(const mt::vec2f& pnt);
//		MATH_INLINE void clear_points();
//
//		MATH_INLINE const vec2f & operator [] ( int i ) const;
//		MATH_INLINE vec2f & operator [] ( int i );
//
//	private:
//		mutable TVectorPoints m_points;
//		mutable mt::mat3f worldMatrix;
//	};
//
//	typedef std::vector< mt::convexpoly2 > TVecConvex;
//
//	MATH_INLINE bool is_point_inside_polygon(TVecConvex & polys, const mt::vec2f & _p, const mt::vec2f & _pos, const mt::vec2f & _dir);
//	MATH_INLINE bool intersect_poly_poly(const convexpoly2 & polygonA, const convexpoly2 & polygonB);
//
//	MATH_INLINE bool is_point_inside_polygon(TVecConvex & polys, const mt::vec2f & _p, const mt::vec2f & _posA, const mt::vec2f & _dirA, const mt::vec2f & _posB, const mt::vec2f & _dirB);
//	MATH_INLINE bool intersect_poly_poly(TVecConvex & polygonsA, TVecConvex & polygonsB,  const mt::vec2f & _posA, const mt::vec2f & _dirA, const mt::vec2f & _posB, const mt::vec2f & _dirB);
//
//	MATH_INLINE bool is_convex_pointsoup(const std::vector<mt::vec2f> & _points);
//	//MATH_INLINE int decompose_concave(std::vector<mt::vec2f> & points, TVecConvex & results);
////	MATH_INLINE int decompose_concave(std::vector<mt::vec2f> & points, std::vector<mt::polygon> & results);
//}
//
//#	if MATH_FORCE_INLINE == 1
//#	include "convexpoly2_inline.h"
//#	endif
