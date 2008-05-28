#	include "mat3.h"

namespace mt
{
	MATH_INLINE convexpoly2::convexpoly2()
	{
		mt::ident_m3(worldMatrix);
	}

	MATH_INLINE convexpoly2::convexpoly2(const std::vector<mt::vec2f> & _points)
	{
		std::copy(_points.begin(),_points.end(),std::back_inserter(m_points));
	}

	MATH_INLINE void convexpoly2::set_transformation(const mt::vec2f& _dirA,const mt::vec2f& _posA)
	{
		worldMatrix.v0 = mt::vec3f(_dirA,1);
		worldMatrix.v1 = mt::vec3f(mt::perp(_dirA),1);
		worldMatrix.v2 = mt::vec3f(_posA,1);
	}

	MATH_INLINE mt::vec2f convexpoly2::get_transformed_vertex(int i) const
	{
		mt::vec2f point;
		mt::mul_v2_m3( point, m_points[i], worldMatrix );
		return point;
	}

	MATH_INLINE mt::vec2f convexpoly2::get_transformed_edge(int i) const
	{
		return get_transformed_vertex((i+1)%num_points()) - get_transformed_vertex(i);
	}

	MATH_INLINE int convexpoly2::num_points() const
	{
		return m_points.size();
	}

	MATH_INLINE void convexpoly2::add_point(const mt::vec2f& pnt)
	{
		m_points.push_back(pnt);
	}

	MATH_INLINE void convexpoly2::clear_points()
	{
		m_points.clear();
	}

	MATH_INLINE const vec2f & convexpoly2::operator [] ( int i ) const
	{
		return m_points[i];
	}

	MATH_INLINE vec2f & convexpoly2::operator [] ( int i )
	{
		return m_points[i];
	}

	MATH_INLINE bool is_point_inside_polygon(mt::convexpoly2& poly, const mt::vec2f& _p)
	{
		int size = poly.num_points();

		if( size == 0 )
		{
			return false;
		}

		convexpoly2::TVectorPoints::size_type intersect_counter = 0;

		mt::vec2f prev;
		prev = poly.get_transformed_vertex(size - 1);

		for( convexpoly2::TVectorPoints::size_type i = 0; i < size; ++i )
		{
			mt::vec2f point;
			point = poly.get_transformed_vertex( i );

			if( ( point.y > _p.y) ^ (prev.y > _p.y) )
			{
				if( prev.x + (_p.y - prev.y) / (point.y - prev.y) * (point.x - prev.x) > _p.x )
				{
					++intersect_counter;
				}
			}
			prev = point;
		}

		return intersect_counter & 1;
	}

	MATH_INLINE void project_polygon(const mt::vec2f& axis, const convexpoly2 & polygon, float & min, float & max) 
	{
		float d = mt::dot_v2_v2(axis,polygon.get_transformed_vertex(0));

		min = d;
		max = d;

		for (int i = 0; i < polygon.num_points(); i++) 
		{
			d = mt::dot_v2_v2(axis,polygon.get_transformed_vertex(i));

			if (d < min) 
			{
				min = d;
			}
			else
			{
				if (d > max)
				{
					max = d;
				}
			}
		}
	}

	MATH_INLINE float interval_distance(float minA, float maxA, float minB, float maxB) 
	{
		return (minA < minB) ? minB - maxA : minA - maxB;
	}

	MATH_INLINE bool intersect_poly_poly(const convexpoly2 & polygonA, const convexpoly2 & polygonB) 
	{
		int edgeCountA = polygonA.num_points();
		int edgeCountB = polygonB.num_points();

		mt::vec2f edge(0,0);

		for (int edgeIndex = 0; edgeIndex < edgeCountA + edgeCountB; edgeIndex++) 
		{
			if (edgeIndex < edgeCountA) 
			{
				edge = polygonA.get_transformed_edge(edgeIndex);
			}
			else
			{
				edge = polygonB.get_transformed_edge(edgeIndex - edgeCountA);
			}

			mt::vec2f axis(-edge.y, edge.x);
			axis.normalize();

			float minA = 0; 
			float minB = 0;
			float maxA = 0;
			float maxB = 0;

			project_polygon(axis, polygonA, minA, maxA);
			project_polygon(axis, polygonB, minB, maxB);

			if (interval_distance(minA, maxA, minB, maxB) > 0) 
			{
				return false;
			}
		}

		return true;
	}

	MATH_INLINE bool is_convex_pointsoup(const std::vector<mt::vec2f> & _points)
	{	   
		if (_points.size() < 3)
		{
			return false;
		}

		int flag = 0;

		float z = 0;

		int n = _points.size();

		for (int i = 0; i < n; i++) 
		{
			int j = (i + 1) % n;
			int k = (i + 2) % n;

			z  = (_points[j].x - _points[i].x) * (_points[k].y - _points[j].y);
			z -= (_points[j].y - _points[i].y) * (_points[k].x - _points[j].x);

			if (z < 0)
			{
				flag |= 1;
			}
			else if (z > 0)
			{
				flag |= 2;
			}

			if (flag == 3)
			{
				return false;
			}
		}

		return (flag != 0);
	}

	MATH_INLINE bool is_point_inside_polygon(TVecConvex & polys, const mt::vec2f & _p, const mt::vec2f & _pos, const mt::vec2f & _dir)
	{
		for(int i = 0; i < polys.size(); i++)
		{
			mt::convexpoly2 & convexpoly = polys[i];

			convexpoly.set_transformation(_dir,_pos);
			bool result = mt::is_point_inside_polygon( convexpoly, _p );

			if(result)
			{
				return true;
			}
		}

		return false;
	}

	MATH_INLINE bool intersect_poly_poly(TVecConvex & polygonsA, TVecConvex & polygonsB,  const mt::vec2f & _posA, const mt::vec2f & _dirA, const mt::vec2f & _posB, const mt::vec2f & _dirB)
	{
		for(int i = 0; i < polygonsA.size(); i++)
		{
			for(int j = 0; j < polygonsB.size(); j++)
			{
				mt::convexpoly2 & convexpolyA = polygonsA[i];
				mt::convexpoly2 & convexpolyB = polygonsB[i];

				convexpolyA.set_transformation(_dirA,_posA);
				convexpolyB.set_transformation(_dirB,_posB);

				bool is_intersect = mt::intersect_poly_poly(convexpolyA, convexpolyB);

				if(is_intersect)
				{
					return true;
				}
			}
		}

		return false;
	}

}