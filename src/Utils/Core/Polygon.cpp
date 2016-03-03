#	include "Polygon.h"

#	include "Core/MemoryAllocator.h"

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
				static inline float get( mt::vec2f const& p )
				{
					return p.template get<Dimension>();
				}

				static inline void set( mt::vec2f & p, float const& value )
				{
					p.template set<Dimension>( value );
				}
			};

		} // namespace traits
	}
}

namespace Menge
{
	typedef boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian> BoostPoint;
	typedef boost::geometry::model::polygon<mt::vec2f, true, true, stdex::vector, stdex::vector, stdex::stl_allocator, stdex::stl_allocator> BoostPolygon;
	typedef boost::geometry::model::box<mt::vec2f> BoostBox;
	typedef boost::geometry::model::segment<mt::vec2f> BoostSegment;

	namespace detail
	{
		static bool s_inside_triangle( float Ax, float Ay,
			float Bx, float By,
			float Cx, float Cy,
			float Px, float Py )
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
			float aCROSSbp = ax*bpy - ay*bpx;
			float cCROSSap = cx*apy - cy*apx;
			float bCROSScp = bx*cpy - by*cpx;

			return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
		}

		static bool s_snip( const BoostPolygon::ring_type & _ring, size_t u, size_t v, size_t w, size_t n, uint32_t * V )
		{
			uint32_t Vu = V[u];
			uint32_t Vv = V[v];
			uint32_t Vw = V[w];

			float Ax = _ring[Vu].x;
			float Ay = _ring[Vu].y;
			float Bx = _ring[Vv].x;
			float By = _ring[Vv].y;
			float Cx = _ring[Vw].x;
			float Cy = _ring[Vw].y;

			float wtf = (((Bx - Ax)*(Cy - Ay)) - ((By - Ay)*(Cx - Ax)));

			if( wtf < mt::m_eps )
			{
				return false;
			}

			for( size_t p = 0; p < n; p++ )
			{
				if( (p == u) || (p == v) || (p == w) )
				{
					continue;
				}

				uint32_t Vp = V[p];

				float Px = _ring[Vp].x;
				float Py = _ring[Vp].y;

				if( s_inside_triangle( Ax, Ay, Bx, By, Cx, Cy, Px, Py ) == true )
				{
					return false;
				}
			}

			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
#	define THIS_IMPL (*(BoostPolygon *)(this->impl))
#	define OTHER_IMPL(other) (*(BoostPolygon *)(other.impl))
	//////////////////////////////////////////////////////////////////////////
	Polygon::Polygon()
	{
		impl = Helper::allocateT<BoostPolygon>();
	}
	//////////////////////////////////////////////////////////////////////////
	Polygon::~Polygon()
	{
		Helper::freeT( (BoostPolygon*)impl);
	}
	//////////////////////////////////////////////////////////////////////////
	Polygon::Polygon( const Polygon & _polygon )
	{
		impl = Helper::allocateT<BoostPolygon>();

		THIS_IMPL = (*(BoostPolygon *)(_polygon.impl));
	}
	//////////////////////////////////////////////////////////////////////////
	Polygon::Polygon( const void * _impl )
	{
		impl = Helper::allocateT<BoostPolygon>();

		THIS_IMPL = (*(BoostPolygon *)(_impl));
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::operator = (const Polygon & _polygon)
	{
		THIS_IMPL = (*(BoostPolygon *)(_polygon.impl));
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::triangulate_indices( TVectorIndices & _result ) const
	{
		/* allocate and initialize list of Vertices in polygon */
		size_t n = boost::geometry::num_points( THIS_IMPL );

		--n; // for correct polygon

		if( n < 3 )
		{
			return false;
		}

		uint32_t * V = Helper::allocateMemory<uint32_t>( n );  /* we want a counter-clockwise polygon in V */

		double area_polygon = boost::geometry::area( THIS_IMPL );

		if( area_polygon < 0.0 )
		{
			for( size_t v = 0; v < n; v++ )
			{
				V[v] = (uint32_t)v;
			}
		}
		else
		{
			for( size_t v = 0; v < n; v++ )
			{
				V[v] = (uint32_t)((n - 1) - v);
			}
		}

		size_t nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
		size_t count = 2 * nv;   /* error detection */

		const BoostPolygon::ring_type & countour = THIS_IMPL.outer();

		for( size_t m = 0, v = nv - 1; nv > 2; )
		{
			/* if we loop, it is probably a non-simple polygon */
			if( 0 == (count--) )
			{
				//** Triangulate: ERROR - probable bad polygon!
				return false;
			}    /* three consecutive vertices in current polygon, <u,v,w> */

			TVectorPoints::size_type u = v;
			if( nv <= u )
			{
				u = 0;     /* previous */
			}

			v = u + 1;

			if( nv <= v )
			{
				v = 0;     /* new v    */
			}

			size_t w = v + 1;
			if( nv <= w )
			{
				w = 0;     /* next     */
			}

			if( detail::s_snip( countour, u, v, w, nv, V ) )
			{
				uint32_t a = V[u];
				uint32_t b = V[v];
				uint32_t c = V[w];

				_result.push_back( (TVectorIndices::value_type)a );
				_result.push_back( (TVectorIndices::value_type)b );
				_result.push_back( (TVectorIndices::value_type)c );

				m++;

				for( size_t s = v, t = v + 1; t < nv; s++, t++ )
				{
					V[s] = V[t];
				}

				nv--;

				count = 2 * nv;
			}
		}

		Helper::freeMemory( V );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::triangulate( TVectorPoints & _result ) const
	{
		/* allocate and initialize list of Vertices in polygon */
		size_t n = boost::geometry::num_points( THIS_IMPL );

		--n; // for correct polygon

		if( n < 3 )
		{
			return false;
		}

		uint32_t *V = Helper::allocateMemory<uint32_t>( n );  /* we want a counter-clockwise polygon in V */

		double area_polygon = boost::geometry::area( THIS_IMPL );

		if( area_polygon < 0.0 )
		{
			for( size_t v = 0; v < n; v++ )
			{
				V[v] = (uint32_t)v;
			}
		}
		else
		{
			for( size_t v = 0; v < n; v++ )
			{
				V[v] = (uint32_t)((n - 1) - v);
			}
		}

		size_t nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
		size_t count = 2 * nv;   /* error detection */

		const BoostPolygon::ring_type & countour = THIS_IMPL.outer();

		for( size_t m = 0, v = nv - 1; nv > 2; )
		{
			/* if we loop, it is probably a non-simple polygon */
			if( 0 == (count--) )
			{
				//** Triangulate: ERROR - probable bad polygon!
				return false;
			}    /* three consecutive vertices in current polygon, <u,v,w> */

			size_t u = v;
			if( nv <= u )
			{
				u = 0;     /* previous */
			}

			v = u + 1;

			if( nv <= v )
			{
				v = 0;     /* new v    */
			}

			size_t w = v + 1;
			if( nv <= w )
			{
				w = 0;     /* next     */
			}

			if( detail::s_snip( countour, u, v, w, nv, V ) )
			{
				uint32_t a = V[u];
				uint32_t b = V[v];
				uint32_t c = V[w];

				const mt::vec2f & Ca = countour[a];
				const mt::vec2f & Cb = countour[b];
				const mt::vec2f & Cc = countour[c];

				_result.push_back( Ca );
				_result.push_back( Cb );
				_result.push_back( Cc );

				m++;

				for( size_t s = v, t = v + 1; t < nv; s++, t++ )
				{
					V[s] = V[t];
				}

				nv--;

				count = 2 * nv;
			}
		}

		Helper::freeMemory( V );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::clear()
	{
		THIS_IMPL.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::append( const mt::vec2f & _v )
	{
		boost::geometry::append( THIS_IMPL, _v );
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::append_inner( const Polygon & _polygon )
	{
		const BoostPolygon::ring_type & ring = OTHER_IMPL( _polygon ).outer();

		boost::geometry::interior_rings( THIS_IMPL ).push_back( ring );
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::correct()
	{
		boost::geometry::correct( THIS_IMPL );
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::mul_wm( Polygon & _out, const mt::mat4f & _wm ) const
	{
		_out.clear();

		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		for( BoostPolygon::ring_type::const_iterator
			it = ring.begin(),
			it_end = ring.end();
		it != it_end;
		++it )
		{
			mt::vec2f v;
			mt::mul_v2_m4( v, *it, _wm );
			_out.append( v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::mul_wm_and_transpose( Polygon & _out, const mt::mat4f & _wm, const mt::vec2f & _pos ) const
	{
		_out.clear();

		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		for( BoostPolygon::ring_type::const_iterator
			it = ring.begin(),
			it_end = ring.end();
		it != it_end;
		++it )
		{
			mt::vec2f v;
			mt::mul_v2_m4( v, *it, _wm );
			v += _pos;

			_out.append( v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Polygon::transpose( Polygon & _out, const mt::vec2f & _pos ) const
	{
		_out.clear();

		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		for( BoostPolygon::ring_type::const_iterator
			it = ring.begin(),
			it_end = ring.end();
		it != it_end;
		++it )
		{
			mt::vec2f v;
			mt::add_v2_v2( v, *it, _pos );
			_out.append( v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::to_box2f( mt::box2f & _box2f ) const
	{
		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		BoostPolygon::ring_type::const_iterator it = ring.begin();
		BoostPolygon::ring_type::const_iterator it_end = ring.end();

		if( it == it_end )
		{
			return false;
		}

		const mt::vec2f & base_v = *it;

		mt::reset( _box2f, base_v );

		++it;

		for( ; it != it_end; ++it )
		{
			const mt::vec2f & v = *it;

			mt::add_internal_point( _box2f, v );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::empty() const
	{
		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		size_t ring_size = ring.size();

		return ring_size == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Polygon::num_points() const
	{
		return boost::geometry::num_points( THIS_IMPL );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Polygon::outer_count() const
	{
		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		size_t ring_size = ring.size();

		return ring_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f * Polygon::outer_points() const
	{
		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		const mt::vec2f * points = &ring[0];

		return points;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f * Polygon::outer_points()
	{
		BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		mt::vec2f * points = &ring[0];

		return points;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Polygon::inners_count() const
	{
		const BoostPolygon::inner_container_type & inners = THIS_IMPL.inners();

		size_t inners_count = inners.size();

		return inners_count;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Polygon::inner_count( size_t _index ) const
	{
		const BoostPolygon::inner_container_type & inners = THIS_IMPL.inners();

		const BoostPolygon::ring_type & ring = inners[_index];

		size_t ring_size = ring.size();

		return ring_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f * Polygon::inner_points( size_t _index ) const
	{
		const BoostPolygon::inner_container_type & inners = THIS_IMPL.inners();

		const BoostPolygon::ring_type & ring = inners[_index];

		const mt::vec2f * points = &ring[0];

		return points;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f * Polygon::inner_points( size_t _index )
	{
		BoostPolygon::inner_container_type & inners = THIS_IMPL.inners();

		BoostPolygon::ring_type & ring = inners[_index];

		mt::vec2f * points = &ring[0];

		return points;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Polygon::inner_point( size_t _index, size_t _v ) const
	{
		const mt::vec2f * v = this->inner_points( _index );

		return v[_v];
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & Polygon::inner_point( size_t _index, size_t _v )
	{
		mt::vec2f * v = this->inner_points( _index );

		return v[_v];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Polygon::outer_point( size_t _index ) const
	{
		const BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		const mt::vec2f & point = ring[_index];

		return point;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & Polygon::outer_point( size_t _index )
	{
		BoostPolygon::ring_type & ring = THIS_IMPL.outer();

		mt::vec2f & point = ring[_index];

		return point;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::intersects( const Polygon & _polygon ) const
	{
		return boost::geometry::intersects( THIS_IMPL, *(BoostPolygon*)(_polygon.impl) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::intersects( const mt::vec2f & _point ) const
	{
		BoostPoint boost_point( _point.x, _point.y );

		return boost::geometry::intersects( THIS_IMPL, boost_point );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::intersects( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const
	{
		BoostSegment boost_segment( _p0, _p1 );

		return boost::geometry::intersects( THIS_IMPL, boost_segment );	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::intersects( const mt::box2f & _box ) const
	{
		BoostBox boost_box( _box.minimum, _box.maximum );

		return boost::geometry::intersects( THIS_IMPL, boost_box );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::intersection( const Polygon & _polygon, TVectorPolygon & _out ) const
	{
		std::deque<BoostPolygon> output;

		try
		{
			boost::geometry::intersection( THIS_IMPL, *(BoostPolygon*)(_polygon.impl), output );
		}
		catch( const std::exception & )
		{
			return false;
		}

		for( std::deque<BoostPolygon>::const_iterator
			it = output.begin(),
			it_end = output.end();
		it != it_end;
		++it )
		{
			const BoostPolygon & polygon = *it;

			_out.push_back( Polygon( &polygon ) );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Polygon::difference( const Polygon & _polygon, TVectorPolygon & _out ) const
	{
		std::deque<BoostPolygon> output;

		try
		{
			boost::geometry::difference( THIS_IMPL, *(BoostPolygon*)(_polygon.impl), output );
		}
		catch( const std::exception & )
		{
			return false;
		}

		for( std::deque<BoostPolygon>::const_iterator
			it = output.begin(),
			it_end = output.end();
		it != it_end;
		++it )
		{
			const BoostPolygon & polygon = *it;

			_out.push_back( Polygon( &polygon ) );
		}

		return true;
	}
}