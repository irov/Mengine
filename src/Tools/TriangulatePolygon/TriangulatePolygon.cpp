#	include <Windows.h>

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <vector>

#	include <sstream>

#	define BOOST_NO_IOSTREAM

#   pragma warning(push, 0) 
#   pragma warning(disable:4800)  

#	include <boost/geometry/geometry.hpp> 
#	include <boost/geometry/core/tag.hpp> 
#	include <boost/geometry/geometries/polygon.hpp>
#	include <boost/geometry/geometries/box.hpp>
#	include <boost/geometry/geometries/point_xy.hpp>
#	include <boost/geometry/geometries/segment.hpp>

#	include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>

#   pragma warning(pop)

#	include "poly2tri/poly2tri.h"

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

	const vec2f & operator = (const vec2f & _v)
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
		return (&x)[i];
	}

	double & operator [] ( size_t i )
	{
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
			{};

			template<size_t Dimension>
			struct access<vec2f, Dimension >
			{
				static inline double get( vec2f const& p )
				{
					return p.template get<Dimension>();
				}

				static inline void set( vec2f & p, double const& value )
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
static void message_error( const char * _format, ... )
{
	va_list argList;

	va_start( argList, _format );

	char str[2048];

	vsnprintf( str, 2048 - 1, _format, argList );

	va_end( argList );

	printf( str );
}
//////////////////////////////////////////////////////////////////////////
class CommandLineStream
{
public:
	CommandLineStream( PWSTR _lpCmdLine )
		: m_cmd_index(0)
	{
		int cmd_num;
		m_cmd_args = CommandLineToArgvW( _lpCmdLine, &cmd_num );
	}

public:
	bool read_b()
	{
		LPWSTR arg = m_cmd_args[m_cmd_index++];

		uint32_t value;
		swscanf( arg, L"%u", &value );

		return value != 0;
	}

	uint32_t read_u()
	{
		LPWSTR arg = m_cmd_args[m_cmd_index++];

		uint32_t value;
		swscanf( arg, L"%u", &value );

		return value;
	}

	double read_f()
	{
		LPWSTR arg = m_cmd_args[m_cmd_index++];

		double value;
		swscanf( arg, L"%lf", &value );

		return value;
	}

	vec2f read_v2()
	{
		LPWSTR arg_x = m_cmd_args[m_cmd_index++];

		double value_x;
		swscanf( arg_x, L"%lf", &value_x );

		LPWSTR arg_y = m_cmd_args[m_cmd_index++];

		double value_y;
		swscanf( arg_y, L"%lf", &value_y );

		return vec2f( value_x, value_y );
	}

protected:
	uint32_t m_cmd_index;
	LPWSTR * m_cmd_args;
};
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

	CommandLineStream stream( lpCmdLine );
		
	vec2f image_base_size = stream.read_v2();
	vec2f image_trim_size = stream.read_v2();
	vec2f image_trim_offset = stream.read_v2();
	
	bool subtract = stream.read_b();

	uint32_t points_count = stream.read_u();

	BoostPolygon polygon;
	
	for( uint32_t points_index = 0; points_index != points_count; ++points_index )
	{
		vec2f v = stream.read_v2();

		boost::geometry::append( polygon, v );
	}

	boost::geometry::correct( polygon );

	vec2f v0( 0.f, 0.f );
	vec2f v1( image_base_size.x, 0.f );
	vec2f v2( image_base_size.x, image_base_size.y );
	vec2f v3( 0.f, image_base_size.y );

	BoostPolygon imagePolygon;
	boost::geometry::append( imagePolygon, v0 );
	boost::geometry::append( imagePolygon, v1 );
	boost::geometry::append( imagePolygon, v2 );
	boost::geometry::append( imagePolygon, v3 );
	boost::geometry::correct( imagePolygon );

	TVectorPolygon output;
	if( subtract == false )
	{
		std::deque<BoostPolygon> result;

		try
		{
			boost::geometry::intersection( polygon, imagePolygon, result );
		}
		catch( const std::exception & )
		{
			message_error( "# " );

			return 0;
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
			boost::geometry::difference( imagePolygon, polygon, result );
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

	if( output.empty() == true )
	{
		printf( "0" );

		return 0;
	}
		
	std::vector<p2t::Point> p2t_points;

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
			const BoostPolygon::inner_container_type & shape_vertex_inners = shape_vertex.inners();

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

		std::vector<p2t::Point*> p2t_polygon;

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
			std::vector<p2t::Point*> p2t_hole;

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

		std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();

		for( std::vector<p2t::Triangle*>::iterator
			it = triangles.begin(),
			it_end = triangles.end();
		it != it_end;
		++it )
		{
			p2t::Triangle* tr = *it;

			p2t::Point * p0 = tr->GetPoint( 0 );
			p2t::Point * p1 = tr->GetPoint( 1 );
			p2t::Point * p2 = tr->GetPoint( 2 );

			p2t::Point * pb = &p2t_points[0];

			uint32_t i0 = (uint32_t)std::distance( pb, p0 );
			uint32_t i1 = (uint32_t)std::distance( pb, p1 );
			uint32_t i2 = (uint32_t)std::distance( pb, p2 );

			shape_indices.push_back( i0 );
			shape_indices.push_back( i1 );
			shape_indices.push_back( i2 );
		}
	}

	size_t shapeVertexCount = p2t_points.size();
	size_t shapeIndicesCount = shape_indices.size();

	uint16_t mesh_vertexCount = (uint16_t)shapeVertexCount;
	uint16_t mesh_indexCount = (uint16_t)shapeIndicesCount;

	vec2f * mesh_pos = new vec2f[shapeVertexCount];
	vec2f * mesh_uv = new vec2f[shapeVertexCount];

	uint16_t * mesh_indices = new uint16_t[shapeIndicesCount];

	for( size_t i = 0; i != shapeVertexCount; ++i )
	{
		const p2t::Point & shape_pos = p2t_points[i];

		mesh_pos[i].x = (float)shape_pos.x;
		mesh_pos[i].y = (float)shape_pos.y;

		mesh_uv[i].x = ((float)shape_pos.x - image_trim_offset.x) / image_trim_size.x;
		mesh_uv[i].y = ((float)shape_pos.y - image_trim_offset.y) / image_trim_size.y;
	}

	for( size_t i = 0; i != shapeIndicesCount; ++i )
	{
		mesh_indices[i] = (uint16_t)shape_indices[i];
	}

	std::stringstream ss;

	ss << mesh_vertexCount << " ";
	ss << mesh_indexCount << " ";

	for( size_t i = 0; i != shapeVertexCount; ++i )
	{
		const vec2f & pos = mesh_pos[i];

		ss << pos.x << " " << pos.y << " ";
	}

	for( size_t i = 0; i != shapeVertexCount; ++i )
	{
		const vec2f & uv = mesh_uv[i];

		ss << uv.x << " " << uv.y << " ";
	}

	for( size_t i = 0; i != shapeIndicesCount; ++i )
	{
		const uint16_t & index = mesh_indices[i];

		ss << index << " ";
	}

	delete[] mesh_pos;
	delete[] mesh_uv;
	delete[] mesh_indices;

	std::string result = ss.str();
	
	printf(
		result.c_str()
		);
	
	return 0;
}