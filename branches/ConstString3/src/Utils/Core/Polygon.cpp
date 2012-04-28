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
			float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
			float cCROSSap, bCROSScp, aCROSSbp;  ax = Cx - Bx;  ay = Cy - By;
			bx = Ax - Cx;  by = Ay - Cy;
			cx = Bx - Ax;  cy = By - Ay;
			apx= Px - Ax;  apy= Py - Ay;
			bpx= Px - Bx;  bpy= Py - By;
			cpx= Px - Cx;  cpy= Py - Cy;  
			aCROSSbp = ax*bpy - ay*bpx;
			cCROSSap = cx*apy - cy*apx;
			bCROSScp = bx*cpy - by*cpx; 
			return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
		}

		static bool s_snip( const Polygon::ring_type & _ring, size_t u, size_t v, size_t w, size_t n, size_t *V )
		{
			size_t p;
			float Ax, Ay, Bx, By, Cx, Cy, Px, Py;  

			Ax = _ring[V[u]].x;
			Ay = _ring[V[u]].y;  
			Bx = _ring[V[v]].x;
			By = _ring[V[v]].y;  
			Cx = _ring[V[w]].x;
			Cy = _ring[V[w]].y; 

			if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) )
			{
				return false; 
			}

			for( p = 0; p < n; p++ )
			{
				if( (p == u) || (p == v) || (p == w) ) 
				{
					continue;
				}

				Px = _ring[V[p]].x;
				Py = _ring[V[p]].y;

				if( s_inside_triangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py) == true )
				{
					return false;
				}
			}  

			return true;
		}
	}

	bool triangulate_polygon(const Polygon & _polygon, 
		TVectorPoints & _result)
	{
		/* allocate and initialize list of Vertices in polygon */ 
		size_t n = boost::geometry::num_points(_polygon);

		if( n == 0 )
		{
			return false;
		}

		n -= 1;

		const Polygon::ring_type & ring = _polygon.outer();

		if( n < 3 )
		{
			return false;
		}

		size_t *V = new size_t[n];  /* we want a counter-clockwise polygon in V */ 

		if( 0.0f > boost::geometry::area(_polygon) )
		{
			for( size_t v=0; v<n; v++ )
			{
				V[v] = v;
			}
		}
		else
		{
			for( size_t v=0; v<n; v++ ) 
			{
				V[v] = (n-1)-v;  
			}
		}

		size_t nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
		size_t count = 2 * nv;   /* error detection */ 

		for( size_t m = 0, v = nv - 1; nv > 2; )
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

			size_t w = v+1;
			if (nv <= w) 
			{
				w = 0;     /* next     */  
			}

			if( detail::s_snip( ring, u, v, w, nv, V ) )
			{
				size_t a,b,c;  
				/* true names of the vertices */
				a = V[u];
				b = V[v]; 
				c = V[w];   
				/* output Triangle */

				_result.push_back( ring[a] );
				_result.push_back( ring[b] );
				_result.push_back( ring[c] );

				m++; 
				/* remove v from remaining polygon */
				for( size_t s = v, t = v + 1; t < nv; s++, t++ ) 
				{
					V[s] = V[t];
				}

				nv--;      /* resest error detection counter */

				count = 2*nv;
			}
		}  

		delete [] V;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void polygon_wm( Polygon & _out, const Polygon & _polygon, const mt::mat4f & _wm )
	{
		size_t n = boost::geometry::num_points(_polygon);

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
	////////////////////////////////////////////////////////////////////////////
	//Vector vectorSub(const Vector& a, const Vector& b)
	//{    
	//	Vector c(a.x-b.x, a.y-b.y);    
	//	return c;
	//}
	//
	//float vectorDot(const Vector& a, const Vector& b)
	//{    
	//	float  d = a.x*b.x + a.y*b.y;    
	//	return d;
	//}
	//
	//float vectorLength(const Vector& a)
	//{    
	//	float length_squared = a.x*a.x + a.y*a.y;    
	//	return sqrt(length_squared);
	//}
	//
	//Vector vectorPerp(const Vector& a)
	//{    
	//	Vector p(-a.y, a.x);    
	//	return p;
	//}
	
	// project polygon along an axis, and find it's dimensions.
	void polygonInterval(const mt::vec2f & axis, const mt::vec2f * a, int anum, float& min, float& max)
	{    
		min = max = mt::dot_v2_v2(a[0], axis);    

		for(int i = 1; i < anum; i ++)    
		{        
			float d = mt::dot_v2_v2(a[i], axis);
			        
			if(d < min) 
				min = d;        
			else if (d > max) 
				max = d;    
		}
	}
	
	// project slphere along an axis, and find it's dimensions.
	void sphereInterval(const mt::vec2f& axis, const mt::vec2f& c, float r, float& min, float& max)
	{    
		float length = mt::length_v2(axis);    
		float cn = mt::dot_v2_v2(axis, c);    

		min = cn - (r * length);    
		max = cn + (r * length);
	}
	
	bool collidePolygonPolygonAxis(const mt::vec2f & axis, const mt::vec2f * a, int anum, const mt::vec2f * b, int bnum)
	{    
		float mina, maxa;    
		float minb, maxb;    

		polygonInterval(axis, a, anum, mina, maxa);    
		polygonInterval(axis, b, bnum, minb, maxb);       
		
		return (mina <= maxb && minb <= maxa);
	}
	
	bool collidePolygonPolygon(const mt::vec2f * a, int anum, const mt::vec2f * b, int bnum)
	{    
		for(int i = 0, j=anum-1; i < anum; j=i, i++)
		{
			mt::vec2f edge;
			mt::sub_v2_v2(edge, a[i], a[j]);

			mt::vec2f axis;
			mt::perp_v2(axis, edge);
			
			// perpendicular to edge             
			if(!collidePolygonPolygonAxis(axis, a, anum, b, bnum))
				return false;    
		} 
		
		for(int i = 0, j=bnum-1; i < bnum; j=i, i++)    
		{
			mt::vec2f edge;
			mt::sub_v2_v2(edge, b[i], b[j]);

			mt::vec2f axis;
			mt::perp_v2(axis, edge);
			
			// perpendicular to edge             
			if(!collidePolygonPolygonAxis(axis, a, anum, b, bnum))             
				return false;    
		} 
		
		return true;
	}
	
	bool collidePolygonSphereAxis(const mt::vec2f & axis, const mt::vec2f * a, int anum, const mt::vec2f & c, float r)
	{ 
		float mina, maxa;
		float minb, maxb;

		polygonInterval(axis, a, anum, mina, maxa);
		sphereInterval(axis, c, r, minb, maxb);
		
		return (mina <= maxb && minb <= maxa);
	}
	
	bool collidePolygonSphere(const mt::vec2f * a, int anum, const mt::vec2f & c, float r)
	{    
		for(int i = 0, j=anum-1; i < anum; j=i, i++)    
		{        
			mt::vec2f edge;
			mt::sub_v2_v2(edge, a[i], a[j]);        

			mt::vec2f axis;
			mt::perp_v2(axis, edge); 
			
			// perpendicular to edge             
			if(!collidePolygonSphereAxis(axis, a, anum, c, r))
				return false;    
		}    
		
		for(int i = 0; i < anum; i++)    
		{        
			mt::vec2f axis;
			mt::sub_v2_v2(axis, c, a[i]); 

			if(!collidePolygonSphereAxis(axis, a, anum, c, r))
				return false;    
		}    
		
		return true;
	}
}