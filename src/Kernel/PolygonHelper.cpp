#include "PolygonHelper.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/StlAllocator.h"
#include "Kernel/Deque.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4800)
#   pragma warning(disable:4702)
#endif
//////////////////////////////////////////////////////////////////////////
#include "boost/geometry/geometry.hpp"
#include "boost/geometry/core/tag.hpp"
#include "boost/geometry/geometries/polygon.hpp"
#include "boost/geometry/geometries/box.hpp"
#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/segment.hpp"

#include "boost/geometry/strategies/agnostic/point_in_poly_winding.hpp"
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop)
#endif
//////////////////////////////////////////////////////////////////////////
#include "math/segment2.h"
#include "stdex/span.h"
//////////////////////////////////////////////////////////////////////////
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
            {
            };

            template<size_t Dimension>
            struct access<mt::vec2f, Dimension >
            {
                static float get( mt::vec2f const & p )
                {
                    return p.template get<Dimension>();
                }

                static void set( mt::vec2f & p, float const & value )
                {
                    p.template set<Dimension>( value );
                }
            };

        }
    }
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static float is_left( const mt::vec2f & p0, const mt::vec2f & p1, const mt::vec2f & p2 )
            {
                return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
            }
            //////////////////////////////////////////////////////////////////////////
            static bool inside_triangle( float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py )
            {
                float ax = Cx - Bx;
                float ay = Cy - By;
                float bx = Ax - Cx;
                float by = Ay - Cy;
                float cx = Bx - Ax;
                float cy = By - Ay;
                float apx = Px - Ax;
                float apy = Py - Ay;
                float bpx = Px - Bx;
                float bpy = Py - By;
                float cpx = Px - Cx;
                float cpy = Py - Cy;
                float aCROSSbp = ax * bpy - ay * bpx;
                float cCROSSap = cx * apy - cy * apx;
                float bCROSScp = bx * cpy - by * cpx;

                return (aCROSSbp >= 0.f) && (bCROSScp >= 0.f) && (cCROSSap >= 0.f);
            }
            //////////////////////////////////////////////////////////////////////////
            static bool snip( const VectorPoints & _points, Polygon::size_type u, Polygon::size_type v, Polygon::size_type w, Polygon::size_type n, Polygon::size_type * const V )
            {
                Polygon::size_type Vu = V[u];
                Polygon::size_type Vv = V[v];
                Polygon::size_type Vw = V[w];

                float Ax = _points[Vu].x;
                float Ay = _points[Vu].y;
                float Bx = _points[Vv].x;
                float By = _points[Vv].y;
                float Cx = _points[Vw].x;
                float Cy = _points[Vw].y;

                float wtf = (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax);

                if( wtf < mt::constant::eps )
                {
                    return false;
                }

                for( Polygon::size_type p = 0; p < n; p++ )
                {
                    if( (p == u) || (p == v) || (p == w) )
                    {
                        continue;
                    }

                    Polygon::size_type Vp = V[p];

                    float Px = _points[Vp].x;
                    float Py = _points[Vp].y;

                    if( Detail::inside_triangle( Ax, Ay, Bx, By, Cx, Cy, Px, Py ) == true )
                    {
                        return false;
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            typedef boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian> BoostPoint;
            typedef boost::geometry::model::polygon<mt::vec2f, true, true, Vector, Vector, StlAllocator, StlAllocator> BoostPolygon;
            typedef boost::geometry::model::box<mt::vec2f> BoostBox;
            typedef boost::geometry::model::segment<mt::vec2f> BoostSegment;
            //////////////////////////////////////////////////////////////////////////
            static void makeBoostPolygon( BoostPolygon * const _bp, const Polygon & _p )
            {
                for( const mt::vec2f & v : _p )
                {
                    boost::geometry::append( *_bp, v );
                }

                boost::geometry::correct( *_bp );
            }
            //////////////////////////////////////////////////////////////////////////
            static void makeMengineGeolygon( Geolygon * const _mg, const BoostPolygon & _bp )
            {
                const BoostPolygon::ring_type & ring = _bp.outer();
                BoostPolygon::ring_type::size_type ring_size = ring.size();

                Polygon outer_polygon;
                outer_polygon.reserve( (Mengine::Polygon::size_type)ring_size );

                for( const mt::vec2f & v : stdex::span::range( ring, 0, -1 ) )
                {
                    outer_polygon.append( v );
                }

                const BoostPolygon::inner_container_type & inners = _bp.inners();

                VectorPolygon inners_polygon;
                for( const BoostPolygon::ring_type & ring_inner : inners )
                {
                    BoostPolygon::ring_type::size_type ring_inner_size = ring_inner.size();

                    Polygon inner_polygon;
                    inner_polygon.reserve( (Mengine::Polygon::size_type)ring_inner_size );

                    for( const mt::vec2f & v : stdex::span::range( ring_inner, 0, -1 ) )
                    {
                        inner_polygon.append( v );
                    }

                    inners_polygon.emplace_back( inner_polygon );
                }

                *_mg = Geolygon( outer_polygon, inners_polygon );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool intersects( const Polygon & _polygon, const mt::vec2f & _point )
        {
            int32_t wn = 0;

            Polygon::size_type points_size = _polygon.size();

            for( Polygon::size_type i = 0; i != points_size; i++ )
            {
                const mt::vec2f & v0 = _polygon[i];
                const mt::vec2f & v1 = _polygon[(i + 1) % points_size];

                if( v0.y <= _point.y )
                {
                    if( v1.y > _point.y )
                    {
                        if( Detail::is_left( v0, v1, _point ) > 0.f )
                        {
                            ++wn;
                        }
                    }
                }
                else
                {
                    if( v1.y <= _point.y )
                    {
                        if( Detail::is_left( v0, v1, _point ) < 0.f )
                        {
                            --wn;
                        }
                    }
                }
            }

            if( wn == 0 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool intersects( const Polygon & _polygon, const mt::vec2f & _point, float _rx, float _ry )
        {
            if( Helper::intersects( _polygon, _point ) == true )
            {
                return true;
            }

            float rxxinv = 1.f / (_rx * _rx);
            float ryyinv = 1.f / (_ry * _ry);

            Polygon::size_type polygon_size = _polygon.size();

            for( Polygon::size_type
                index = 0,
                index_end = polygon_size;
                index != index_end;
                ++index )
            {
                const mt::vec2f & v0 = _polygon[index + 0];
                const mt::vec2f & v1 = _polygon[(index + 1) % polygon_size];

                mt::vec2f projection;
                mt::segment2_projection_point( mt::segment2( v0, v1 ), _point, projection );

                mt::vec2f dp = _point - projection;

                float d = (dp.x * dp.x) * rxxinv + (dp.y * dp.y) * ryyinv;

                if( d <= 1.f )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        void centroid( const Polygon & _polygon, mt::vec2f * const _point )
        {
            mt::vec2f centroid_point( 0.f, 0.f );
            float centroid_area = 0.f;

            // For all vertices except last
            const VectorPoints & polygon_vertices = _polygon.getPoints();
            Polygon::size_type polygon_count = _polygon.size();

            for( Polygon::size_type i = 0; i != polygon_count - 1; ++i )
            {
                float x0 = polygon_vertices[i + 0].x;
                float y0 = polygon_vertices[i + 0].y;
                float x1 = polygon_vertices[i + 1].x;
                float y1 = polygon_vertices[i + 1].y;

                float a = x0 * y1 - x1 * y0;

                centroid_area += a;

                centroid_point.x += (x0 + x1) * a;
                centroid_point.y += (y0 + y1) * a;
            }

            {
                float x0 = polygon_vertices[polygon_count - 1].x;
                float y0 = polygon_vertices[polygon_count - 1].y;
                float x1 = polygon_vertices[0].x;
                float y1 = polygon_vertices[0].y;

                float a = x0 * y1 - x1 * y0;

                centroid_area += a;

                centroid_point.x += (x0 + x1) * a;
                centroid_point.y += (y0 + y1) * a;
            }

            centroid_area *= 0.5f;

            float centroid_area_inv6 = 6.f / centroid_area;

            centroid_point.x *= centroid_area_inv6;
            centroid_point.y *= centroid_area_inv6;

            *_point = centroid_point;
        }
        //////////////////////////////////////////////////////////////////////////
        bool triangulate( const Polygon & _polygon, VectorPoints * const _result )
        {
            /* allocate and initialize list of Vertices in polygon */
            const VectorPoints & polygon_points = _polygon.getPoints();
            uint32_t polygon_size = _polygon.size();

            --polygon_size; // for correct polygon

            if( polygon_size < 3 )
            {
                return false;
            }

            uint32_t * V = Helper::allocateArrayT<uint32_t>( polygon_size );  /* we want a counter-clockwise polygon in V */

            float polygon_area = _polygon.area();

            if( polygon_area < 0.f )
            {
                for( uint32_t v = 0; v != polygon_size; ++v )
                {
                    V[v] = v;
                }
            }
            else
            {
                for( uint32_t v = 0; v != polygon_size; ++v )
                {
                    V[v] = (polygon_size - 1) - v;
                }
            }

            uint32_t nv = polygon_size;
            uint32_t count = 2 * nv;

            for( uint32_t v = nv - 1; nv > 2; )
            {
                if( 0 == (count--) )
                {
                    Helper::freeArrayT( V );

                    return false;
                }

                uint32_t u = v;

                if( nv <= u )
                {
                    u = 0;
                }

                v = u + 1;

                if( nv <= v )
                {
                    v = 0;
                }

                uint32_t w = v + 1;

                if( nv <= w )
                {
                    w = 0;
                }

                if( Detail::snip( polygon_points, u, v, w, nv, V ) == true )
                {
                    uint32_t a = V[u];
                    uint32_t b = V[v];
                    uint32_t c = V[w];

                    const mt::vec2f & Ca = polygon_points[a];
                    const mt::vec2f & Cb = polygon_points[b];
                    const mt::vec2f & Cc = polygon_points[c];

                    _result->emplace_back( Ca );
                    _result->emplace_back( Cb );
                    _result->emplace_back( Cc );

                    for( uint32_t s = v, t = v + 1; t < nv; s++, t++ )
                    {
                        V[s] = V[t];
                    }

                    --nv;

                    count = 2 * nv;
                }
            }

            Helper::freeArrayT( V );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool triangulate_indices( const Polygon & _polygon, VectorPolygonIndices * const _result )
        {
            const VectorPoints & polygon_points = _polygon.getPoints();
            uint32_t n = _polygon.size();

            --n;

            if( n < 3 )
            {
                return false;
            }

            uint32_t * V = Helper::allocateArrayT<uint32_t>( n );

            float polygon_area = _polygon.area();

            if( polygon_area < 0.f )
            {
                for( uint32_t v = 0; v < n; v++ )
                {
                    V[v] = v;
                }
            }
            else
            {
                for( uint32_t v = 0; v < n; v++ )
                {
                    V[v] = (n - 1) - v;
                }
            }

            uint32_t nv = n;
            uint32_t count = 2 * nv;

            for( uint32_t v = nv - 1; nv > 2; )
            {
                if( 0 == (count--) )
                {
                    Helper::freeArrayT( V );

                    return false;
                }

                uint32_t u = v;

                if( nv <= u )
                {
                    u = 0;
                }

                v = u + 1;

                if( nv <= v )
                {
                    v = 0;
                }

                uint32_t w = v + 1;

                if( nv <= w )
                {
                    w = 0;
                }

                if( Detail::snip( polygon_points, u, v, w, nv, V ) == true )
                {
                    uint32_t a = V[u];
                    uint32_t b = V[v];
                    uint32_t c = V[w];

                    _result->emplace_back( a );
                    _result->emplace_back( b );
                    _result->emplace_back( c );

                    for( uint32_t s = v, t = v + 1; t < nv; s++, t++ )
                    {
                        V[s] = V[t];
                    }

                    nv--;

                    count = 2 * nv;
                }
            }

            Helper::freeArrayT( V );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool intersects( const Polygon & _polygon, const mt::vec2f & _p0, const mt::vec2f & _p1 )
        {
            Detail::BoostPolygon bpolygon;
            Detail::makeBoostPolygon( &bpolygon, _polygon );

            Detail::BoostSegment boost_segment( _p0, _p1 );

            bool successful = boost::geometry::intersects( bpolygon, boost_segment );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool intersects( const Polygon & _lhs, const Polygon & _rhs )
        {
            Detail::BoostPolygon blhs;
            Detail::makeBoostPolygon( &blhs, _lhs );

            Detail::BoostPolygon brhs;
            Detail::makeBoostPolygon( &brhs, _rhs );

            bool successful = boost::geometry::intersects( blhs, brhs );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool intersects( const Polygon & _lhs, const mt::box2f & _rhs )
        {
            Detail::BoostPolygon blhs;
            Detail::makeBoostPolygon( &blhs, _lhs );

            Detail::BoostBox boost_box( _rhs.minimum, _rhs.maximum );

            bool successful = boost::geometry::intersects( blhs, boost_box );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool intersection( const Polygon & _lhs, const Polygon & _rhs, VectorGeolygon * const _out )
        {
            Detail::BoostPolygon blhs;
            Detail::makeBoostPolygon( &blhs, _lhs );

            Detail::BoostPolygon brhs;
            Detail::makeBoostPolygon( &brhs, _rhs );

            Deque<Detail::BoostPolygon> output;

            try
            {
                boost::geometry::intersection( blhs, brhs, output );
            }
            catch( const std::exception & )
            {
                return false;
            }

            for( const Detail::BoostPolygon & bp : output )
            {
                Geolygon mg;
                Detail::makeMengineGeolygon( &mg, bp );

                _out->emplace_back( mg );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool difference( const Polygon & _lhs, const Polygon & _rhs, VectorGeolygon * const _out )
        {
            Detail::BoostPolygon blhs;
            Detail::makeBoostPolygon( &blhs, _lhs );

            Detail::BoostPolygon brhs;
            Detail::makeBoostPolygon( &brhs, _rhs );

            Deque<Detail::BoostPolygon> output;

            try
            {
                boost::geometry::difference( blhs, brhs, output );
            }
            catch( const std::exception & )
            {
                return false;
            }

            for( const Detail::BoostPolygon & bp : output )
            {
                Geolygon mg;
                Detail::makeMengineGeolygon( &mg, bp );

                _out->emplace_back( mg );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}