#	include	"triangulation.h"
#	include	"polygon.h"

namespace mt
{
	static const float EPSILON = 0.0000000001f;
	
	static bool inside_triangle(float Ax, float Ay,
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
	
	static bool snip(const std::vector<mt::vec2f> &contour,std::size_t u,std::size_t v,std::size_t w,std::size_t n,std::size_t *V)
	{
		std::size_t p;
		float Ax, Ay, Bx, By, Cx, Cy, Px, Py;  
		
		Ax = contour[V[u]].x;
		Ay = contour[V[u]].y;  Bx = contour[V[v]].x;
		By = contour[V[v]].y;  Cx = contour[V[w]].x;
		Cy = contour[V[w]].y; 

		if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) )
			return false; 

		for (p=0;p<n;p++)
		{
			if( (p == u) || (p == v) || (p == w) ) 
				continue;

			Px = contour[V[p]].x;
			Py = contour[V[p]].y;

			if (inside_triangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) 
				return false;
		}  
		return true;
	}

	bool triangulate_polygon(const std::vector<mt::vec2f> & contour, 
		std::vector<mt::vec2f> & result)
	{
		/* allocate and initialize list of Vertices in polygon */ 
		std::vector<mt::vec2f>::size_type n = contour.size();
		if ( n < 3 ) return false;
		
		std::size_t *V = new std::size_t[n];  /* we want a counter-clockwise polygon in V */ 

		if ( 0.0f < polygon_area(contour) )
		for (std::size_t v=0; v<n; v++)
			V[v] = v;
		else
		for(std::size_t v=0; v<n; v++) 
			V[v] = (n-1)-v;  
		
		std::vector<mt::vec2f>::size_type nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
		std::vector<mt::vec2f>::size_type count = 2 * nv;   /* error detection */ 
		
		for(std::vector<mt::vec2f>::size_type m = 0, v = nv - 1; nv > 2; )
		{
			/* if we loop, it is probably a non-simple polygon */
			if (0 >= (count--))
			{
			//** Triangulate: ERROR - probable bad polygon!
				return false;
			}    /* three consecutive vertices in current polygon, <u,v,w> */

			std::vector<mt::vec2f>::size_type u = v  ;
			if (nv <= u) u = 0;     /* previous */
			v = u+1;

			if (nv <= v) v = 0;     /* new v    */

			std::vector<mt::vec2f>::size_type w = v+1;
			if (nv <= w) w = 0;     /* next     */  

			if ( snip(contour,u,v,w,nv,V) )
			{
				std::size_t a,b,c;  
				/* true names of the vertices */
				a = V[u];
				b = V[v]; 
				c = V[w];   
				/* output Triangle */
				result.push_back( contour[a] );
				result.push_back( contour[b] );
				result.push_back( contour[c] );    
				m++; 
				/* remove v from remaining polygon */
				for(std::vector<mt::vec2f>::size_type s=v,t=v+1;t<nv;s++,t++) 
					V[s] = V[t]; nv--;      /* resest error detection counter */

				count = 2*nv;
			}
		}  

		delete V; 
		return true;
	}

	MATH_INLINE bool triangulate_polygon(const mt::polygon & _polygon,
			  std::vector<mt::vec2f> & _result)
	{
		return triangulate_polygon( _polygon.get_points(), _result);
	}

}