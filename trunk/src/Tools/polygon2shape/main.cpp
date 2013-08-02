#   include <stdio.h>
#   include <vector>

#   include "Core/Polygon.h"

static bool sscanf_polygons( const char * _line, Menge::Polygon & _polygon )
{
    size_t len = strlen( _line );
    char * parse_value = new char[len + 1];
    strcpy( parse_value, _line );

    char * pch = strtok( parse_value, " " );

    while( pch != nullptr )
    {
        float x = 0.f;
        if( sscanf( pch, "%f", &x ) != 1 )
        {
            return false;
        }
        
        pch = strtok( nullptr, " " );

        float y = 0.f;
        if( sscanf( pch, "%f", &y ) != 1 )
        {
            return false;
        }

        pch = strtok( nullptr, " " );

        boost::geometry::append( _polygon, mt::vec2f(x, y) );
    }

    delete [] parse_value;

    boost::geometry::correct( _polygon );

    return true;
}

static bool get_shape( FILE * _file, Menge::Polygon & _polygon, Menge::TVectorIndices & _idencies )
{
    char line[1024];
    char * l_polygon1 = fgets( line, 1024, _file );

    if( l_polygon1 == nullptr )
    {
        printf("invalid get 'polygon 1' line\n"
            );

        return false;
    }

    Menge::Polygon polygon1;
    sscanf_polygons( l_polygon1, polygon1 );

    char * l_polygon2 = fgets( line, 1024, _file );

    if( l_polygon2 == nullptr )
    {
        printf("invalid get 'polygon 2' line\n"
            );

        return false;
    }

    Menge::Polygon polygon2;
    sscanf_polygons( l_polygon2, polygon2 );

    std::deque<Menge::Polygon> output;
    boost::geometry::intersection( polygon1, polygon2, output );

    Menge::Polygon polygon_intersect = output[0];

    Menge::TVectorIndices shape_indices;

    if( Menge::triangulate_polygon_indices( polygon_intersect, shape_indices ) == false )
    {
        printf("triangulate failed\n"
            );

        return false;
    }

    size_t polygonIntersectionCount = boost::geometry::num_points( polygon_intersect );

    if( polygonIntersectionCount == 0 )
    {
        return false;
    }

    _polygon = polygon_intersect;
    _idencies = shape_indices;    

    return true;
}

static void write_shape( FILE * f_out, const Menge::Polygon & _polygon, const Menge::TVectorIndices & _indices )
{
    const Menge::Polygon::ring_type & vertices = _polygon.outer();

    size_t vertices_count = vertices.size() - 1;

    fprintf( f_out, "%d\n", vertices_count );    

    for( size_t i = 0; i != vertices_count; ++i )
    {
        const mt::vec2f & v = vertices[i];

        fprintf( f_out, "%.10f %.10f "
            , v.x
            , v.y
            );
    }

    fprintf( f_out, "\n" );

    size_t indices_count = _indices.size();

    fprintf( f_out, "%d\n", indices_count );

    for( size_t i = 0; i != indices_count; ++i )
    {
        size_t ind = _indices[i];

        fprintf( f_out, "%d "
            , ind
            );
    }

    fprintf( f_out, "\n" );
}

int main( int argc, const char *argv[] )
{
    if( argc != 3 )
    {
        printf("need 2 arg, 'in' and 'out' file path\n"
            );

        return 0;
    }

    const char * file_in = argv[1];

    FILE * f_in = fopen( file_in, "r" );

    if( f_in == nullptr )
    {
        printf("file %s not found\n"
            , file_in
            );

        return 0;
    }

    Menge::Polygon polygon;
    Menge::TVectorIndices indices;

    bool shape_successful = get_shape( f_in, polygon, indices );

    fclose( f_in ); 

    if( shape_successful == false )
    {
        return 0;
    }

    const char * file_out = argv[2];

    FILE * f_out = fopen( file_out, "w" );

    if( f_out == nullptr )
    {
        printf("file %s not found\n"
            , file_out
            );

        return 0;
    }

    write_shape( f_out, polygon, indices );

    fclose( f_out );

    return 0;
}