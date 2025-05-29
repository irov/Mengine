#include "Config/Config.h"
#include "ToolUtils/ToolUtils.h"

#include "clipper2/clipper.h"
#include "mapbox/earcut.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <sstream>
#include <limits>

struct vec2d
{
    double x{0.0}, y{0.0};
};

using VectorPoints = std::vector<vec2d>;
using VectorIndices = std::vector<uint32_t>;

struct Mesh
{
    uint16_t              vertexCount;
    uint16_t              indexCount;
    std::vector<vec2d>    pos;
    std::vector<vec2d>    uv;
    std::vector<uint16_t> indices;
};

static Clipper2Lib::PathD to_pathd( const VectorPoints & pts )
{
    Clipper2Lib::PathD p;
    p.reserve( pts.size() );
    for( const vec2d & v : pts ) p.emplace_back( v.x, v.y );
    return p;
}

static Clipper2Lib::PathD build_rect( double x, double y, double w, double h )
{
    return {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};
}

static void classify( const Clipper2Lib::PathsD & in, Clipper2Lib::PathsD & hulls, Clipper2Lib::PathsD & holes )
{
    for( const Clipper2Lib::PathD & p : in )
    {
        (Clipper2Lib::IsPositive( p ) ? hulls : holes).push_back( p );
    }
}

using MeshPart = std::pair<std::vector<double>, std::vector<uint32_t>>;

static MeshPart earcut_polygon( const Clipper2Lib::PathD & outer, const Clipper2Lib::PathsD & holes )
{
    using Point = std::array<double, 2>;
    using Polygon = std::vector<Point>;
    using Polygons = std::vector<Polygon>;

    Polygons poly;
    Polygon  outerRing;
    for( auto & p : outer ) 
    {
        outerRing.push_back( {p.x, p.y} );
    }
    poly.push_back( std::move( outerRing ) );

    for( const Clipper2Lib::PathD & h : holes )
    {
        Polygon hole;
        for( auto & p : h ) hole.push_back( {p.x, p.y} );
        poly.push_back( std::move( hole ) );
    }

    std::vector<uint32_t> idx = mapbox::earcut<uint32_t>( poly );

    std::vector<double> verts;
    verts.reserve( poly.size() * 6 );
    for( const Polygon & ring : poly )
    {
        for( const Point & pt : ring )
        {
            verts.push_back( pt[0] ); 
            verts.push_back( pt[1] );
        }
    }

    return {std::move( verts ), std::move( idx )};
}

static Mesh build_mesh( const Clipper2Lib::PathsD & hulls, const Clipper2Lib::PathsD & holes, const vec2d & trim_size, const vec2d & trim_off )
{
    std::vector<MeshPart> parts;
    for( const Clipper2Lib::PathD & hull : hulls ) 
    {
        parts.push_back( earcut_polygon( hull, holes ) );
    }

    struct PairHash
    {
        size_t operator()( const std::pair<double, double> & p ) const noexcept
        {
            return std::hash<double>{}(p.first) ^ (std::hash<double>{}(p.second) << 1);
        }
    };

    std::unordered_map<std::pair<double, double>, uint32_t, PairHash> lookup;
    VectorPoints vertices;
    VectorIndices indices;

    for( const MeshPart & pr : parts )
    {
        const std::vector<double> & v = pr.first;
        const std::vector<uint32_t> & idx = pr.second;
        
        for( uint32_t i : idx )
        {
            auto key = std::make_pair( v[i * 2], v[i * 2 + 1] );
            auto [it, ins] = lookup.try_emplace( key, static_cast<uint32_t>(vertices.size()) );
            if( ins )
            {
                vertices.push_back( {key.first, key.second} );
            }
            indices.push_back( static_cast<uint32_t>(it->second) );
        }
    }

    Mesh m;
    m.vertexCount = static_cast<uint16_t>(vertices.size());
    m.indexCount = static_cast<uint16_t>(indices.size());
    m.pos = vertices;
    m.uv.resize( vertices.size() );
    m.indices.resize( indices.size() );

    for( size_t i = 0; i < vertices.size(); ++i )
    {
        m.uv[i].x = (vertices[i].x - trim_off.x) / trim_size.x;
        m.uv[i].y = (vertices[i].y - trim_off.y) / trim_size.y;
    }
    for( size_t i = 0; i < indices.size(); ++i ) m.indices[i] = static_cast<uint16_t>( indices[i] );

    return m;
}

static bool polygonize( const vec2d & base_size, const vec2d & trim_size, const vec2d & trim_off, bool bb, bool subtract, const VectorPoints & pts, Mesh & mesh )
{
    (void)base_size;

    Clipper2Lib::PathD mask = to_pathd( pts );
    Clipper2Lib::PathsD clipped;

    if( bb )
    {
        Clipper2Lib::PathD rect = build_rect( trim_off.x, trim_off.y, trim_size.x, trim_size.y );
        clipped = apply_mask( rect, mask, subtract );
    }
    else
    {
        clipped = {mask};
    }

    if( clipped.empty() )
    {
        mesh.vertexCount = mesh.indexCount = 0;
        return true; // no geometry but not an error
    }

    Clipper2Lib::PathsD hulls, holes;
    classify( clipped, hulls, holes );

    mesh = build_mesh( hulls, holes, trim_size, trim_off );
    return true;
}

static void write_result( FILE * fp, const Mesh & mesh )
{
    ::fprintf_s( fp, "%u\n", mesh.vertexCount );
    ::fprintf_s( fp, "%u\n", mesh.indexCount );
    ::fprintf_s( fp, "" );

    for( size_t i = 0; i != mesh.vertexCount; ++i )
    {
        if( i != 0 )
        {
            ::fprintf_s( fp, ", " );
        }

        const vec2d & v = mesh.pos[i];

        ::fprintf_s( fp, "%12f, %12f"
            , v.x
            , v.y
        );
    }

    ::fprintf_s( fp, "\n" );

    ::fprintf_s( fp, "" );

    for( size_t i = 0; i != mesh.vertexCount; ++i )
    {
        if( i != 0 )
        {
            ::fprintf_s( fp, ", " );
        }

        const vec2d & uv = mesh.uv[i];

        ::fprintf_s( fp, "%12f, %12f"
            , uv.x
            , uv.y
        );
    }

    ::fprintf_s( fp, "\n" );

    ::fprintf_s( fp, "" );

    for( uint16_t i = 0; i != mesh.indexCount; ++i )
    {
        if( i != 0 )
        {
            fprintf_s( fp, ", " );
        }

        uint32_t index = mesh.indices[i];

        fprintf_s( fp, "%u"
            , index
        );
    }
}

bool parse_points( const std::vector<double> & points, VectorPoints & points2 )
{
    if( points.size() % 2 != 0 )
    {
        return false;
    }

    points2.reserve( points.size() / 2 );

    for( size_t i = 0; i < points.size(); i += 2 )
    {
        vec2d v;
        v.x = points[i];
        v.y = points[i + 1];
        points2.push_back( v );
    }

    return true;
}

int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    PWSTR pwCmdLine = ::GetCommandLineW();

    bool image_bb = parse_kwds( pwCmdLine, L"--bb", true );
    bool image_subtract = parse_kwds( pwCmdLine, L"--subtract", false );

    double base_width = parse_kwds( pwCmdLine, L"--base_width", 0.0 );
    double base_height = parse_kwds( pwCmdLine, L"--base_height", 0.0 );
    double trim_width = parse_kwds( pwCmdLine, L"--trim_width", 0.0 );
    double trim_height = parse_kwds( pwCmdLine, L"--trim_height", 0.0 );
    double trim_offset_x = parse_kwds( pwCmdLine, L"--trim_offset_x", 0.0 );
    double trim_offset_y = parse_kwds( pwCmdLine, L"--trim_offset_y", 0.0 );

    std::wstring result_path = parse_kwds( pwCmdLine, L"--result_path", std::wstring() );

    std::vector<double> points;
    if( parse_vector_kwds( pwCmdLine, L"--points", points ) == false )
    {
        message_error( "not found --points" );

        return EXIT_FAILURE;
    }

    if( points.empty() == true )
    {
        message_error( "empty --points data" );

        return EXIT_FAILURE;
    }

    vec2d image_base_size{base_width, base_height};
    vec2d image_trim_size{trim_width, trim_height};
    vec2d image_trim_offset{trim_offset_x, trim_offset_y};

    VectorPoints points2;
    if( parse_points( points, points2 ) == false )
    {
        message_error( "invalid --points data" );

        return EXIT_FAILURE;
    }

    Mesh mesh;
    if( polygonize( image_base_size, image_trim_size, image_trim_offset, image_bb, image_subtract, points2, mesh ) == false )
    {
        message_error( "polygonize failed" );

        return EXIT_FAILURE;
    }

    if( result_path.empty() == false )
    {
        WCHAR filePath[MAX_PATH];
        ForcePathQuoteSpaces( filePath, result_path.c_str() );
        PathUnquoteSpaces( filePath );

        FILE * fp;
        if( _wfopen_s( &fp, filePath, L"wt" ) != 0 )
        {
            message_error( "invalid _wfopen %ls", filePath );
            return EXIT_FAILURE;
        }
        
        write_result( fp, mesh );

        fclose( fp );
    }
    else
    {
        write_result( stdout, mesh );
    }

    return EXIT_SUCCESS;
}