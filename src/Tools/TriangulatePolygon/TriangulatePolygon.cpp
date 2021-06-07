#include <Windows.h>

#include <shellapi.h>

#include <stdio.h>

#include <string>
#include <vector>

#include <sstream>

#include "ToolUtils/ToolUtils.h"

#define BOOST_NO_IOSTREAM

#pragma warning(push, 0) 
#pragma warning(disable:4800)  

#include "boost/geometry/geometry.hpp"
#include "boost/geometry/core/tag.hpp"
#include "boost/geometry/geometries/polygon.hpp"
#include "boost/geometry/geometries/box.hpp"
#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/segment.hpp"

#include "boost/geometry/strategies/agnostic/point_in_poly_winding.hpp"

#pragma warning(pop)

#include "poly2tri/poly2tri.h"

struct vec2f
{
    vec2f()
    {
    };

    vec2f( const vec2f & _v )
        : x( _v.x )
        , y( _v.y )
    {
    };

    const vec2f & operator = ( const vec2f & _v )
    {
        x = _v.x;
        y = _v.y;

        return *this;
    }

    vec2f( double _x, double _y )
        : x( _x )
        , y( _y )
    {
    };

    double operator [] ( size_t i ) const
    {
        //cppcheck-suppress objectIndex
        return (&x)[i];
    }

    double & operator [] ( size_t i )
    {
        //cppcheck-suppress objectIndex
        return (&x)[i];
    }

    template <int K>
    double get() const
    {
        return this->operator [] ( K );
    }

    template <int K>
    void set( double _value )
    {
        this->operator [] ( K ) = _value;
    }

    double x;
    double y;
};

namespace boost
{
    namespace geometry
    {
        namespace traits
        {
            template <>
            struct tag<vec2f>
            {
                typedef boost::geometry::point_tag type;
            };

            template<>
            struct coordinate_type<vec2f>
            {
                typedef double type;
            };

            template<>
            struct coordinate_system<vec2f>
            {
                typedef boost::geometry::cs::cartesian type;
            };

            template<>
            struct dimension<vec2f>
                : boost::mpl::int_<2>
            {
            };

            template<size_t Dimension>
            struct access<vec2f, Dimension >
            {
                static inline double get( vec2f const & p )
                {
                    return p.template get<Dimension>();
                }

                static inline void set( vec2f & p, double const & value )
                {
                    p.template set<Dimension>( value );
                }
            };

        } // namespace traits
    }
}

typedef boost::geometry::model::polygon<vec2f, true, true> BoostPolygon;

typedef std::vector<BoostPolygon> TVectorPolygon;

typedef std::vector<vec2f> TVectorPoints;
typedef std::vector<uint32_t> TVectorIndices;

//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, TVectorPoints & _value )
{
    std::vector<std::wstring> tokens;

    std::wstringstream wss( _str );

    std::wstring buf;
    while( wss >> buf )
    {
        tokens.push_back( buf );
    }

    std::vector<std::wstring>::size_type count = tokens.size();

    if( count % 2 != 0 )
    {
        return;
    }

    for( uint32_t i = 0; i != count; i += 2 )
    {
        const wchar_t * tocken_0 = tokens[i + 0].c_str();
        const wchar_t * tocken_1 = tokens[i + 1].c_str();

        double value_x;
        swscanf( tocken_0, L"%lf", &value_x );

        double value_y;
        swscanf( tocken_1, L"%lf", &value_y );

        vec2f v( (float)value_x, (float)value_y );
        _value.push_back( v );
    }
}
//////////////////////////////////////////////////////////////////////////
struct Mesh
{
    uint16_t vertexCount;
    uint16_t indexCount;

    std::vector<vec2f> pos;
    std::vector<vec2f> uv;
    std::vector<uint16_t> indices;
};
//////////////////////////////////////////////////////////////////////////
static bool polygonize( const vec2f & image_base_size, const vec2f & image_trim_size, const vec2f & image_trim_offset, bool image_bb, bool image_subtract, const TVectorPoints & points, Mesh & mesh )
{
    (void)image_base_size;

    size_t points_count = points.size();

    BoostPolygon polygon_input;

    for( size_t points_index = 0; points_index != points_count; ++points_index )
    {
        const vec2f & v = points[points_index];

        boost::geometry::append( polygon_input, v );
    }

    boost::geometry::correct( polygon_input );

    TVectorPolygon output;

    if( image_bb == true )
    {
        vec2f v0( image_trim_offset.x, image_trim_offset.y );
        vec2f v1( image_trim_offset.x + image_trim_size.x, image_trim_offset.y );
        vec2f v2( image_trim_offset.x + image_trim_size.x, image_trim_offset.y + image_trim_size.y );
        vec2f v3( image_trim_offset.x, image_trim_offset.y + image_trim_size.y );

        BoostPolygon imagePolygon;
        boost::geometry::append( imagePolygon, v0 );
        boost::geometry::append( imagePolygon, v1 );
        boost::geometry::append( imagePolygon, v2 );
        boost::geometry::append( imagePolygon, v3 );
        boost::geometry::correct( imagePolygon );

        if( image_subtract == false )
        {
            std::deque<BoostPolygon> result;

            try
            {
                boost::geometry::intersection( polygon_input, imagePolygon, result );
            }
            catch( const std::exception & )
            {
                return false;
            }

            for( std::deque<BoostPolygon>::const_iterator
                it = result.begin(),
                it_end = result.end();
                it != it_end;
                ++it )
            {
                const BoostPolygon & polygon = *it;

                output.push_back( polygon );
            }
        }
        else
        {
            std::deque<BoostPolygon> result;

            try
            {
                boost::geometry::difference( imagePolygon, polygon_input, result );
            }
            catch( const std::exception & )
            {
                return false;
            }

            for( std::deque<BoostPolygon>::const_iterator
                it = result.begin(),
                it_end = result.end();
                it != it_end;
                ++it )
            {
                const BoostPolygon & polygon = *it;

                output.push_back( polygon );
            }
        }
    }
    else
    {
        output.push_back( polygon_input );
    }

    std::vector<p2t::Point> p2t_points;
    std::vector<p2t::Point *> p2t_polygon_trinagles;

    size_t max_points = 0;

    for( TVectorPolygon::const_iterator
        it = output.begin(),
        it_end = output.end();
        it != it_end;
        ++it )
    {
        const BoostPolygon & shape_vertex = *it;

        const BoostPolygon::ring_type & shape_vertex_ring = shape_vertex.outer();
        size_t outer_count = shape_vertex_ring.size();

        max_points += outer_count - 1;

        const BoostPolygon::inner_container_type & shape_vertex_inners = shape_vertex.inners();
        size_t inners_count = shape_vertex_inners.size();

        for( size_t index = 0; index != inners_count; ++index )
        {
            const BoostPolygon::ring_type & shape_vertex_inners_ring = shape_vertex_inners[index];

            size_t inner_count = shape_vertex_inners_ring.size();

            max_points += inner_count - 1;
        }
    }

    p2t_points.reserve( max_points );

    TVectorIndices shape_indices;

    for( TVectorPolygon::const_iterator
        it = output.begin(),
        it_end = output.end();
        it != it_end;
        ++it )
    {
        const BoostPolygon & shape_vertex = *it;

        std::vector<p2t::Point *> p2t_polygon;

        const BoostPolygon::ring_type & shape_vertex_ring = shape_vertex.outer();

        size_t outer_count = shape_vertex_ring.size();

        for( size_t index = 0; index != outer_count - 1; ++index )
        {
            const vec2f & v = shape_vertex_ring[index];

            p2t_points.push_back( p2t::Point( v.x, v.y ) );
            p2t::Point * p = &p2t_points.back();

            p2t_polygon.push_back( p );
        }

        p2t::CDT cdt( p2t_polygon );

        const BoostPolygon::inner_container_type & shape_vertex_inners = shape_vertex.inners();

        size_t inners_count = shape_vertex_inners.size();

        for( size_t index_inners = 0; index_inners != inners_count; ++index_inners )
        {
            std::vector<p2t::Point *> p2t_hole;

            const BoostPolygon::ring_type & shape_vertex_inners_ring = shape_vertex_inners[index_inners];

            size_t inner_count = shape_vertex_inners_ring.size();

            for( size_t index_inner = 0; index_inner != inner_count - 1; ++index_inner )
            {
                const vec2f & v = shape_vertex_inners_ring[index_inner];

                p2t_points.push_back( p2t::Point( v.x, v.y ) );
                p2t::Point * p = &p2t_points.back();

                p2t_hole.push_back( p );
            }

            cdt.AddHole( p2t_hole );
        }

        cdt.Triangulate();

        std::vector<p2t::Triangle *> triangles = cdt.GetTriangles();

        for( std::vector<p2t::Triangle *>::iterator
            it_triangle = triangles.begin(),
            it_triangle_end = triangles.end();
            it_triangle != it_triangle_end;
            ++it_triangle )
        {
            p2t::Triangle * tr = *it_triangle;

            p2t::Point * p0 = tr->GetPoint( 0 );
            p2t::Point * p1 = tr->GetPoint( 1 );
            p2t::Point * p2 = tr->GetPoint( 2 );

            if( std::find( p2t_polygon_trinagles.begin(), p2t_polygon_trinagles.end(), p0 ) == p2t_polygon_trinagles.end() )
            {
                p2t_polygon_trinagles.push_back( p0 );
            }

            if( std::find( p2t_polygon_trinagles.begin(), p2t_polygon_trinagles.end(), p1 ) == p2t_polygon_trinagles.end() )
            {
                p2t_polygon_trinagles.push_back( p1 );
            }

            if( std::find( p2t_polygon_trinagles.begin(), p2t_polygon_trinagles.end(), p2 ) == p2t_polygon_trinagles.end() )
            {
                p2t_polygon_trinagles.push_back( p2 );
            }
        }

        for( std::vector<p2t::Triangle *>::iterator
            it_triangle = triangles.begin(),
            it_triangle_end = triangles.end();
            it_triangle != it_triangle_end;
            ++it_triangle )
        {
            p2t::Triangle * tr = *it_triangle;

            p2t::Point * p0 = tr->GetPoint( 0 );
            p2t::Point * p1 = tr->GetPoint( 1 );
            p2t::Point * p2 = tr->GetPoint( 2 );

            uint32_t i0 = (uint32_t)std::distance( p2t_polygon_trinagles.begin(), std::find( p2t_polygon_trinagles.begin(), p2t_polygon_trinagles.end(), p0 ) );
            uint32_t i1 = (uint32_t)std::distance( p2t_polygon_trinagles.begin(), std::find( p2t_polygon_trinagles.begin(), p2t_polygon_trinagles.end(), p1 ) );
            uint32_t i2 = (uint32_t)std::distance( p2t_polygon_trinagles.begin(), std::find( p2t_polygon_trinagles.begin(), p2t_polygon_trinagles.end(), p2 ) );

            shape_indices.push_back( i0 );
            shape_indices.push_back( i1 );
            shape_indices.push_back( i2 );
        }
    }

    if( output.empty() == true )
    {
        mesh.vertexCount = 0U;
        mesh.indexCount = 0U;
    }
    else
    {
        size_t shapeVertexCount = p2t_polygon_trinagles.size();
        size_t shapeIndicesCount = shape_indices.size();

        mesh.vertexCount = (uint16_t)shapeVertexCount;
        mesh.indexCount = (uint16_t)shapeIndicesCount;

        mesh.pos.resize( shapeVertexCount );
        mesh.uv.resize( shapeVertexCount );

        mesh.indices.resize( shapeIndicesCount );

        for( size_t i = 0; i != shapeVertexCount; ++i )
        {
            const p2t::Point * shape_pos = p2t_polygon_trinagles[i];

            mesh.pos[i].x = (float)shape_pos->x;
            mesh.pos[i].y = (float)shape_pos->y;

            mesh.uv[i].x = ((float)shape_pos->x - image_trim_offset.x) / image_trim_size.x;
            mesh.uv[i].y = ((float)shape_pos->y - image_trim_offset.y) / image_trim_size.y;
        }

        for( size_t i = 0; i != shapeIndicesCount; ++i )
        {
            mesh.indices[i] = (uint16_t)shape_indices[i];
        }
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    std::wstring arg_image_bb;
    std::wstring arg_image_base_size;
    std::wstring arg_image_trim_size;
    std::wstring arg_image_trim_offset;
    std::wstring arg_polygon_input;

    bool image_bb = parse_kwds( lpCmdLine, L"--bb", false );
    float base_width = parse_kwds( lpCmdLine, L"--base_width", 0.f );
    float base_height = parse_kwds( lpCmdLine, L"--base_height", 0.f );
    float trim_width = parse_kwds( lpCmdLine, L"--trim_width", 0.f );
    float trim_height = parse_kwds( lpCmdLine, L"--trim_height", 0.f );
    float trim_offset_x = parse_kwds( lpCmdLine, L"--trim_offset_x", 0.f );
    float trim_offset_y = parse_kwds( lpCmdLine, L"--trim_offset_y", 0.f );
    bool image_subtract = parse_kwds( lpCmdLine, L"--subtract", false );
    TVectorPoints points = parse_kwds( lpCmdLine, L"--points", TVectorPoints() );
    std::wstring result_path = parse_kwds( lpCmdLine, L"--result_path", std::wstring() );

    vec2f image_base_size( base_width, base_height );
    vec2f image_trim_size( trim_width, trim_height );
    vec2f image_trim_offset( trim_offset_x, trim_offset_y );

    Mesh mesh;
    bool successful = polygonize( image_base_size, image_trim_size, image_trim_offset, image_bb, image_subtract, points, mesh );

    WCHAR infoCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( infoCanonicalizeQuote, result_path.c_str() );
    PathUnquoteSpaces( infoCanonicalizeQuote );

    FILE * f_result;
    errno_t err = _wfopen_s( &f_result, infoCanonicalizeQuote, L"wt" );

    if( err != 0 )
    {
        message_error( "invalid _wfopen %ls err %d"
            , infoCanonicalizeQuote
            , err
        );

        return 0;
    }

    if( successful == false )
    {
        fprintf_s( f_result, "# invalid polygonize\n" );
    }
    else
    {
        fprintf_s( f_result, "%u\n", mesh.vertexCount );
        fprintf_s( f_result, "%u\n", mesh.indexCount );

        fprintf_s( f_result, "" );

        for( uint16_t i = 0; i != mesh.vertexCount; ++i )
        {
            if( i != 0 )
            {
                fprintf_s( f_result, "," );
            }

            const vec2f & pos = mesh.pos[i];

            fprintf_s( f_result, " %12f, %12f"
                , pos.x
                , pos.y
            );
        }

        fprintf_s( f_result, "\n" );

        fprintf_s( f_result, "" );

        for( uint16_t i = 0; i != mesh.vertexCount; ++i )
        {
            if( i != 0 )
            {
                fprintf_s( f_result, "," );
            }

            const vec2f & uv = mesh.uv[i];

            fprintf_s( f_result, " %12f, %12f"
                , uv.x
                , uv.y
            );
        }

        fprintf_s( f_result, "\n" );

        fprintf_s( f_result, "" );

        for( uint16_t i = 0; i != mesh.indexCount; ++i )
        {
            if( i != 0 )
            {
                fprintf_s( f_result, "," );
            }

            uint16_t index = mesh.indices[i];

            fprintf_s( f_result, " %u"
                , index
            );
        }

        fprintf_s( f_result, "\n" );
    }

    fclose( f_result );

    return 0;
}