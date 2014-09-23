#	include "Polygon.h"

namespace Menge
{
	static const float EPSILON = 0.0000000001f;

	namespace detail
	{
		static bool s_inside_triangle(float Ax, float Ay,
			float Bx, float By,
			float Cx, float Cy,
			float Px, float Py)
		{
            float ax = Cx - Bx;  
            float ay = Cy - By;
			float bx = Ax - Cx;  
            float by = Ay - Cy;
			float cx = Bx - Ax;  
            float cy = By - Ay;
			float apx= Px - Ax;  
            float apy= Py - Ay;
			float bpx= Px - Bx;  
            float bpy= Py - By;
			float cpx= Px - Cx;  
            float cpy= Py - Cy;  
			float aCROSSbp = ax*bpy - ay*bpx;
			float cCROSSap = cx*apy - cy*apx;
			float bCROSScp = bx*cpy - by*cpx; 

			return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
		}

		static bool s_snip( const Polygon::ring_type & _ring, uint32_t u, uint32_t v, uint32_t w, uint32_t n, uint32_t * V )
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

            float wtf = (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax)));

			if ( wtf < EPSILON )
			{
				return false; 
			}

			for( uint32_t p = 0; p < n; p++ )
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

    bool triangulate_polygon_indices( const Polygon & _polygon, TVectorIndices & _result )
    {
		/* allocate and initialize list of Vertices in polygon */ 
		uint32_t n = boost::geometry::num_points(_polygon);

        --n; // for correct polygon

        if( n < 3 )
        {
            return false;
        }

		uint32_t *V = new uint32_t[n];  /* we want a counter-clockwise polygon in V */ 

        double area_polygon = boost::geometry::area( _polygon );

		if( area_polygon < 0.0 )
		{
			for( uint32_t v=0; v<n; v++ )
			{
				V[v] = v;
			}
		}
		else
		{
			for( uint32_t v=0; v<n; v++ ) 
			{
				V[v] = (n-1)-v;  
			}
		}

		uint32_t nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
		uint32_t count = 2 * nv;   /* error detection */ 

        const Polygon::ring_type & countour = _polygon.outer();
        
		for( uint32_t m = 0, v = nv - 1; nv > 2; )
		{
			/* if we loop, it is probably a non-simple polygon */
			if (0 == (count--))
			{
				//** Triangulate: ERROR - probable bad polygon!
				return false;
			}    /* three consecutive vertices in current polygon, <u,v,w> */

			TVectorPoints::size_type u = v;
			if( nv <= u ) 
			{
				u = 0;     /* previous */
			}

			v = u+1;

			if( nv <= v )
			{
				v = 0;     /* new v    */
			}

			uint32_t w = v+1;
			if (nv <= w) 
			{
				w = 0;     /* next     */  
			}

			if( detail::s_snip( countour, u, v, w, nv, V ) )
			{
				uint32_t a = V[u];
				uint32_t b = V[v]; 
				uint32_t c = V[w];
                
				_result.push_back( (uint16_t)a );
				_result.push_back( (uint16_t)b );
				_result.push_back( (uint16_t)c );

				m++; 

                for( uint32_t s = v, t = v + 1; t < nv; s++, t++ ) 
				{
					V[s] = V[t];
				}

				nv--;

				count = 2*nv;
			}
		}  

		delete [] V;

		return true;
    }

	bool triangulate_polygon(const Polygon & _polygon, TVectorPoints & _result)
	{
		/* allocate and initialize list of Vertices in polygon */ 
		uint32_t n = boost::geometry::num_points(_polygon);

        --n; // for correct polygon

        if( n < 3 )
        {
            return false;
        }

		uint32_t *V = new uint32_t[n];  /* we want a counter-clockwise polygon in V */ 

        double area_polygon = boost::geometry::area( _polygon );

		if( area_polygon < 0.0 )
		{
			for( uint32_t v=0; v<n; v++ )
			{
				V[v] = v;
			}
		}
		else
		{
			for( uint32_t v=0; v<n; v++ ) 
			{
				V[v] = (n-1)-v;  
			}
		}

		uint32_t nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
		uint32_t count = 2 * nv;   /* error detection */ 

        const Polygon::ring_type & countour = _polygon.outer();
        
		for( uint32_t m = 0, v = nv - 1; nv > 2; )
		{
			/* if we loop, it is probably a non-simple polygon */
			if (0 == (count--))
			{
				//** Triangulate: ERROR - probable bad polygon!
				return false;
			}    /* three consecutive vertices in current polygon, <u,v,w> */

			TVectorPoints::size_type u = v;
			if( nv <= u ) 
			{
				u = 0;     /* previous */
			}

			v = u+1;

			if( nv <= v )
			{
				v = 0;     /* new v    */
			}

			uint32_t w = v+1;
			if (nv <= w) 
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

                for( uint32_t s = v, t = v + 1; t < nv; s++, t++ ) 
				{
					V[s] = V[t];
				}

				nv--;

				count = 2*nv;
			}
		}  

		delete [] V;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void polygon_wm( Polygon & _out, const Polygon & _polygon, const mt::mat4f & _wm )
	{
		_out.clear();

		const Polygon::ring_type & ring = _polygon.outer();

		for( Polygon::ring_type::const_iterator
			it = ring.begin(),
		it_end = ring.end();
		it != it_end;
		++it )
		{
			mt::vec2f v;
			mt::mul_v2_m4(v, *it, _wm);
			boost::geometry::append( _out, v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void polygon_wm_and_transpose( Polygon & _out, const Polygon & _polygon, const mt::mat4f & _wm, const mt::vec2f & _pos )
	{
		_out.clear();

		const Polygon::ring_type & ring = _polygon.outer();

		for( Polygon::ring_type::const_iterator
			it = ring.begin(),
			it_end = ring.end();
		it != it_end;
		++it )
		{
			mt::vec2f v;
			mt::mul_v2_m4(v, *it, _wm);
			v += _pos;

			boost::geometry::append( _out, v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void polygon_transpose( Polygon & _out, const Polygon & _polygon, const mt::vec2f & _pos )
	{
		_out.clear();

		const Polygon::ring_type & ring = _polygon.outer();

		for( Polygon::ring_type::const_iterator
			it = ring.begin(),
			it_end = ring.end();
		it != it_end;
		++it )
		{
			mt::vec2f v;
			mt::add_v2_v2( v, *it, _pos );
			boost::geometry::append( _out, v );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool polygon_to_box2f( mt::box2f & _box2f, const Polygon & _polygon )
    {
        const Polygon::ring_type & ring = _polygon.outer();

        Polygon::ring_type::const_iterator it = ring.begin();
        Polygon::ring_type::const_iterator it_end = ring.end();

        if( it == it_end )
        {
            return false;
        }

        const mt::vec2f & v = *it;

        mt::reset( _box2f, v );

        ++it;
        
        for( ; it != it_end; ++it )
        {
            const mt::vec2f & v = *it;

            mt::add_internal_point( _box2f, v );
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t polygon_size( const Polygon & _polygon )
	{
		const Polygon::ring_type & ring = _polygon.outer();

		uint32_t ring_size = ring.size();

		return ring_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f * polygon_points( const Polygon & _polygon )
	{
		const Polygon::ring_type & ring = _polygon.outer();

		const mt::vec2f * points = &ring[0];

		return points;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t polygon_inners( const Polygon & _polygon )
	{
		const Polygon::inner_container_type & inners = _polygon.inners();

		uint32_t inners_count = inners.size();

		return inners_count;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t polygon_inner_size( const Polygon & _polygon, uint32_t _index )
	{
		const Polygon::inner_container_type & inners = _polygon.inners();

		const Polygon::ring_type & ring = inners[_index];

		uint32_t ring_size = ring.size();

		return ring_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f * polygon_inner_points( const Polygon & _polygon, uint32_t _index )
	{
		const Polygon::inner_container_type & inners = _polygon.inners();

		const Polygon::ring_type & ring = inners[_index];

		const mt::vec2f * points = &ring[0];

		return points;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & polygon_point( const Polygon & _polygon, uint32_t _index )
	{
		const Polygon::ring_type & ring = _polygon.outer();

		const mt::vec2f & point = ring[_index];

		return point;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_pnpoly( uint32_t nvert, const mt::vec2f * vert, float testx, float testy )
	{
		bool c = false;

		for( uint32_t i = 0, j = nvert - 1; i < nvert; j = i++ )
		{
			float vertix = vert[i].x;
			float vertiy = vert[i].y;
			float vertjx = vert[j].x;
			float vertjy = vert[j].y;

			if( ( (vertiy > testy) != (vertjy > testy) ) &&
				( testx < (vertjx - vertix) * (testy-vertiy) / (vertjy - vertiy) + vertix) )
			{
				c = !c;
			}
		}

		return c;
	}
	//////////////////////////////////////////////////////////////////////////
	bool intersection_polygon_point( const Polygon & _polygon, float _x, float _y )
	{
		uint32_t vertex_count = polygon_size( _polygon );
		const mt::vec2f * points = polygon_points( _polygon );

		if( s_pnpoly( vertex_count, points, _x, _y ) == false )
		{
			return false;
		}

		uint32_t inners = polygon_inners( _polygon );

		for( uint32_t k = 0; k != inners; ++k )
		{
			uint32_t vertex_count = polygon_inner_size( _polygon, k );
			const mt::vec2f * points = polygon_inner_points( _polygon, k );

			if( s_pnpoly( vertex_count, points, _x, _y ) == true )
			{
				return false;
			}
		}

		return true;
	}
}