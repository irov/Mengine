#	pragma once

#	include "Math/vec2.h"
#	include "Math/mat4.h"
#   include "Math/box2.h"

#pragma warning(push, 0) 
#	include <boost/geometry/geometry.hpp> 
#	include <boost/geometry/core/tag.hpp> 
#	include <boost/geometry/geometries/polygon.hpp>
#	include <boost/geometry/geometries/box.hpp>

#	include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>
#pragma warning(pop)

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
    typedef std::vector<mt::vec2f> TVectorPoints;	

    typedef boost::geometry::model::polygon<mt::vec2f> Polygon;
    typedef boost::geometry::model::box<mt::vec2f> Box;

    bool triangulate_polygon( const Polygon & _polygon, TVectorPoints & _result );

    void polygon_wm( Polygon & _out, const Polygon & _polygon, const mt::mat4f & _wm );
    bool polygon_to_box2f( mt::box2f & _box2f, const Polygon & _polygon );
}