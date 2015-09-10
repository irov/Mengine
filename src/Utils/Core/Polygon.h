#	pragma once

#   include "Config/Typedef.h"

#	include "Math/vec2.h"
#	include "Math/mat4.h"
#   include "Math/box2.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4800)  
#   endif

#	include <boost/geometry/geometry.hpp> 
#	include <boost/geometry/core/tag.hpp> 
#	include <boost/geometry/geometries/polygon.hpp>
#	include <boost/geometry/geometries/box.hpp>
#	include <boost/geometry/geometries/point_xy.hpp>
#	include <boost/geometry/geometries/segment.hpp>

#	include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop)
#   endif

#	include <stdex/stl_vector.h>

namespace boost 
{
    namespace geometry
    {
        namespace traits
        {
            template <>
            struct tag<mt::vec2f>
            {
                typedef boost::geometry::point_tag type;
            };

            template<>
            struct coordinate_type<mt::vec2f>
            {
                typedef float type;
            };

            template<>
            struct coordinate_system<mt::vec2f>
            {
                typedef boost::geometry::cs::cartesian type;
            };

            template<>
            struct dimension<mt::vec2f>
                : boost::mpl::int_<2>
            {};

            template<size_t Dimension>
            struct access<mt::vec2f, Dimension >
            {
                static inline float get(mt::vec2f const& p)
                {
                    return p.template get<Dimension>();
                }

                static inline void set(mt::vec2f & p, float const& value)
                {
                    p.template set<Dimension>(value);
                }
            };

        } // namespace traits
    }
}

namespace Menge
{
    typedef stdex::vector<mt::vec2f> TVectorPoints;
    typedef stdex::vector<uint32_t> TVectorIndices;

	typedef boost::geometry::model::d2::point_xy<float> GeometryPoint;
    typedef boost::geometry::model::polygon<mt::vec2f, true, true, stdex::vector, stdex::vector, stdex::stl_allocator, stdex::stl_allocator> Polygon;
    typedef boost::geometry::model::box<mt::vec2f> Box;
	typedef boost::geometry::model::segment<mt::vec2f> Segment;

	typedef stdex::vector<Polygon> TVectorPolygon;

    bool triangulate_polygon( const Polygon & _polygon, TVectorPoints & _result );
    bool triangulate_polygon_indices( const Polygon & _polygon, TVectorIndices & _result );
	
    void polygon_wm( Polygon & _out, const Polygon & _polygon, const mt::mat4f & _wm );
	void polygon_wm_and_transpose( Polygon & _out, const Polygon & _polygon, const mt::mat4f & _wm, const mt::vec2f & _pos );
	void polygon_transpose( Polygon & _out, const Polygon & _polygon, const mt::vec2f & _pos );

    bool polygon_to_box2f( mt::box2f & _box2f, const Polygon & _polygon );
	bool polygon_empty( const Polygon & _polygon );
	uint32_t polygon_size( const Polygon & _polygon );
	uint32_t polygon_inners( const Polygon & _polygon );
	uint32_t polygon_inner_size( const Polygon & _polygon, uint32_t _index );
	const mt::vec2f * polygon_inner_points( const Polygon & _polygon, uint32_t _index );
	const mt::vec2f * polygon_points( const Polygon & _polygon );
	const mt::vec2f & polygon_point( const Polygon & _polygon, uint32_t _index );

	bool intersection_polygon_point( const Polygon & _polygon, float _x, float _y );
}