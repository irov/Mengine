#	include "PathFinderMap.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap::PathFinderMap()
		: m_cachePointUse(0)
		, m_unitSize(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap::~PathFinderMap()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setMap( const Polygon & _polygon )
	{
		m_mapPolygon = _polygon;

		boost::geometry::correct( m_mapPolygon );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setUnitSize( float _size )
	{
		m_unitSize = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_enlargePolygonFromLow( Polygon & _big, float _radius )
	{
		size_t numpoints = boost::geometry::num_points( _big );

		if( numpoints == 0 )
		{
			return;
		}

		--numpoints;

		const Polygon::ring_type & ring = _big.outer();

		for( size_t i = 0; i != numpoints; ++i )
		{
			const mt::vec2f & p0 = ring[(i-1) % numpoints];
			const mt::vec2f & p1 = ring[(i+0) % numpoints];
			const mt::vec2f & p2 = ring[(i+1) % numpoints];

			mt::vec2f v0 = p1 - p0;
			mt::vec2f v0_norm;
			mt::norm_v2( v0_norm, v0 );
			mt::vec2f v0_norm_perp;
			mt::perp_v2( v0_norm_perp, v0_norm );

			mt::vec2f v1 = p2 - p1;
			mt::vec2f v1_norm;
			mt::norm_v2( v1_norm, v1 );
			mt::vec2f v1_norm_perp;
			mt::perp_v2( v1_norm_perp, v1_norm );

			mt::vec2f d = v0_norm_perp + v1_norm_perp;
			mt::vec2f d_norm;
			mt::norm_v2( d_norm, d );

			d_norm *= _radius;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::addHole( size_t _index, const Polygon & _polygon )
	{
		Polygon big_polygon = _polygon;
		boost::geometry::correct( big_polygon );

		s_enlargePolygonFromLow( big_polygon, m_unitSize );		

		if( this->testHoles_( _index, big_polygon ) == true )
		{
			return false;
		}

		m_bigHoles[_index] = big_polygon;

		Polygon low_polygon = _polygon;
		boost::geometry::correct( low_polygon );
		m_holes[_index] = low_polygon;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testHoles_( size_t _index, const Polygon & _polygon ) const
	{
		for( THoles::const_iterator
			it = m_bigHoles.begin(),
			it_end = m_bigHoles.end();
		it != it_end;
		++it )
		{
			size_t hole_index = it->first;

			if( hole_index == _index )
			{
				continue;
			}

			const Polygon & p = it->second;

			bool intersect = boost::geometry::intersects( p, _polygon );

			if( intersect == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_makePolyPointFromPolygon( Poly2Tri::Point * _cachePoint, size_t & _cachePointUse, const Polygon & _polygon, Poly2Tri::TVectorPoint & _points )
	{
		size_t numpoints = boost::geometry::num_points( _polygon );

		if( numpoints == 0 )
		{
			return false;
		}

		--numpoints;

		const Polygon::ring_type & ring = _polygon.outer();

		for( size_t
			it = 0,
			it_end = numpoints;
		it != it_end;
		++it )
		{
			const mt::vec2f & v = ring[it];

			if( _cachePointUse >= 1024 )
			{
				return false;
			}

			Poly2Tri::Point & p = _cachePoint[_cachePointUse];
			++_cachePointUse;

			p.x = v.x;
			p.y = v.y;
			p.weight = 0.f;
			p.edge_list.clear();

			_points.push_back( &p );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::generateMap()
	{
		Poly2Tri::TVectorPoint mapPoints;
		s_makePolyPointFromPolygon( m_cachePoint, m_cachePointUse, m_mapPolygon, mapPoints );

		m_sweepContext.setPoint( mapPoints );

		for( THoles::const_iterator
			it = m_bigHoles.begin(),
			it_end = m_bigHoles.end();
		it != it_end;
		++it )
		{
			const Polygon & p = it->second;

			Poly2Tri::TVectorPoint holePoint;
			s_makePolyPointFromPolygon( m_cachePoint, m_cachePointUse, p, holePoint );

			m_sweepContext.AddHole( holePoint );
		}

		Poly2Tri::Sweep sweep;
		sweep.Triangulate(m_sweepContext);
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static float s_makePointDeterm( float x1, float y1, float x2, float y2, float x3, float y3 )
	{
		float D = (x3 - x1) * (y2 - y1) - (y3 - y1) * (x2 - x1);

		return D;
	}
	//////////////////////////////////////////////////////////////////////////
	static const Poly2Tri::Triangle * s_findTriangle( const Poly2Tri::TVectorTriangle & _triangles, const mt::vec2f & _point )
	{
		for( Poly2Tri::TVectorTriangle::const_iterator
			it = _triangles.begin(),
			it_end = _triangles.end();
		it != it_end;
		++it )
		{
			const Poly2Tri::Triangle * tr = *it;

			Poly2Tri::Point * p0 = tr->GetPoint(0);
			Poly2Tri::Point * p1 = tr->GetPoint(1);
			Poly2Tri::Point * p2 = tr->GetPoint(2);

			
			if( s_makePointDeterm( p0->x, p0->y, p1->x, p1->y, _point.x, _point.y ) > 0.f )
			{
				continue;
			}
			
			if( s_makePointDeterm( p1->x, p1->y, p2->x, p2->y, _point.x, _point.y ) > 0.f )
			{
				continue;
			}

			if( s_makePointDeterm( p2->x, p2->y, p0->x, p0->y, _point.x, _point.y ) > 0.f )
			{
				continue;
			}

			return tr;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static float s_wavePathPoint( float _x, float _y, Poly2Tri::Point * _point )
	{
		float wavelength = sqrtf( (_point->x - _x) * (_point->x - _x) + (_point->y - _y) * (_point->y - _y) );

		return wavelength;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wavePathTriangle( const Poly2Tri::Triangle * _triangle )
	{
		if( _triangle == nullptr )
		{
			return;
		}

		if( _triangle->IsInterior() == false )
		{
			return;
		}

		Poly2Tri::Point * p0 = _triangle->GetPoint( 0 );
		Poly2Tri::Point * p1 = _triangle->GetPoint( 1 );
		Poly2Tri::Point * p2 = _triangle->GetPoint( 2 );
				
		float w01 = s_wavePathPoint( p0->x, p0->y, p1 );
		float w12 = s_wavePathPoint( p1->x, p1->y, p2 );
		float w20 = s_wavePathPoint( p2->x, p2->y, p0 );
				
		bool accept = false;
		//////////////////////////////////////////////////////////////////////////
		if( p1->weight > p0->weight + w01 + 1.f )
		{
			p1->weight = p0->weight + w01;

			accept = true;
		}

		if( p0->weight > p1->weight + w01 + 1.f )
		{
			p0->weight = p1->weight + w01;

			accept = true;
		}

		//////////////////////////////////////////////////////////////////////////
		if( p2->weight > p0->weight + w20 + 1.f )
		{
			p2->weight = p0->weight + w20;

			accept = true;
		}

		if( p0->weight > p2->weight + w20 + 1.f )
		{
			p0->weight = p2->weight + w20;

			accept = true;
		}

		//////////////////////////////////////////////////////////////////////////
		if( p1->weight > p2->weight + w12 + 1.f )
		{
			p1->weight = p2->weight + w12;

			accept = true;
		}

		if( p2->weight > p1->weight + w12 + 1.f )
		{
			p2->weight = p1->weight + w12;

			accept = true;
		}

		if( accept == false )
		{
			return;
		}

		Poly2Tri::Triangle * n0 = _triangle->GetNeighbor( 0 );
		Poly2Tri::Triangle * n1 = _triangle->GetNeighbor( 1 );
		Poly2Tri::Triangle * n2 = _triangle->GetNeighbor( 2 );

		s_wavePathTriangle( n0 );
		s_wavePathTriangle( n1 );
		s_wavePathTriangle( n2 );
	}
	//////////////////////////////////////////////////////////////////////////
	static Poly2Tri::Point * s_findMinimal( Poly2Tri::Point * _p0, Poly2Tri::Point * _p1, Poly2Tri::Point * _p2)
	{
		if( _p0->weight < _p1->weight )
		{
			if( _p0->weight < _p2->weight )
			{
				return _p0;
			}
			else
			{
				return _p2;
			}
		}
		else
		{
			if( _p1->weight < _p2->weight )
			{
				return _p1;
			}
			else
			{
				return _p2;
			}
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static Poly2Tri::Point * s_findMinimalTriangle( Poly2Tri::Triangle * _tr )
	{
		if( _tr == nullptr || _tr->IsInterior() == false )
		{
			return nullptr;
		}

		Poly2Tri::Point * p0 = _tr->GetPoint( 0 );
		Poly2Tri::Point * p1 = _tr->GetPoint( 1 );
		Poly2Tri::Point * p2 = _tr->GetPoint( 2 );

		if( p0->weight < p1->weight )
		{
			if( p0->weight < p2->weight )
			{
				return p0;
			}
			else
			{
				return p2;
			}
		}
		else
		{
			if( p1->weight < p2->weight )
			{
				return p1;
			}
			else
			{
				return p2;
			}
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<Poly2Tri::Point *> TVectorPathPoint;
	//////////////////////////////////////////////////////////////////////////
	static void s_slitherPath( Poly2Tri::Point * _pt, const Poly2Tri::Triangle * _from, const Poly2Tri::Triangle * _to, TVectorPathPoint & _path )
	{
		if( _from == _to )
		{
			return;
		}

		Poly2Tri::Point * p_from0 = _from->GetPoint( 0 );
		Poly2Tri::Point * p_from1 = _from->GetPoint( 1 );
		Poly2Tri::Point * p_from2 = _from->GetPoint( 2 );

		Poly2Tri::Point * p_min = s_findMinimal( p_from0, p_from1, p_from2 );

		Poly2Tri::Triangle * tr_neihgbor0 = _from->GetNeighbor( 0 );
		Poly2Tri::Triangle * tr_neihgbor1 = _from->GetNeighbor( 1 );
		Poly2Tri::Triangle * tr_neihgbor2 = _from->GetNeighbor( 2 );

		Poly2Tri::Point * p0_min = s_findMinimalTriangle( tr_neihgbor0 );
		Poly2Tri::Point * p1_min = s_findMinimalTriangle( tr_neihgbor1 );
		Poly2Tri::Point * p2_min = s_findMinimalTriangle( tr_neihgbor2 );

		//float wp_min = p_min->weight;

		float w0_min = p0_min ? p0_min->weight : 1024.f * 1024.f;
		float w1_min = p1_min ? p1_min->weight : 1024.f * 1024.f;
		float w2_min = p2_min ? p2_min->weight : 1024.f * 1024.f;

		if( w0_min <= w1_min && w0_min <= w2_min )
		{
			if( p0_min == nullptr )
			{
				return;
			}
			
			if( _pt != p0_min )
			{
				_path.push_back( p0_min );
			}

			s_slitherPath( p0_min, tr_neihgbor0, _to, _path );
		}
		else if( w1_min <= w0_min && w1_min <= w2_min )
		{
			if( p1_min == nullptr )
			{
				return;
			}
			
			if( _pt != p1_min )
			{
				_path.push_back( p1_min );
			}

			s_slitherPath( p1_min, tr_neihgbor1, _to, _path );
		}
		else if( w2_min <= w0_min && w2_min <= w1_min )
		{
			if( p2_min == nullptr )
			{
				return;
			}

			if( _pt != p2_min )
			{
				_path.push_back( p2_min );
			}

			s_slitherPath( p2_min, tr_neihgbor2, _to, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::findPath( const mt::vec2f & _from, const mt::vec2f & _to )
	{
		const Poly2Tri::TVectorTriangle & triangles = m_sweepContext.GetTriangles();

		const Poly2Tri::Triangle * tr_from = s_findTriangle( triangles, _from );
		const Poly2Tri::Triangle * tr_to = s_findTriangle( triangles, _to );

		if( tr_to == nullptr || tr_to->IsInterior() == false || tr_from == nullptr || tr_from->IsInterior() == false )
		{
			return false;
		}
		
		for( size_t i = 0; i != m_cachePointUse; ++i )
		{
			m_cachePoint[i].weight = 1024.f * 1024.f;
		}
		
		Poly2Tri::Point * p_to0 = tr_to->GetPoint( 0 );
		Poly2Tri::Point * p_to1 = tr_to->GetPoint( 1 );
		Poly2Tri::Point * p_to2 = tr_to->GetPoint( 2 );

		p_to0->weight = s_wavePathPoint( _to.x, _to.y, p_to0 );
		p_to1->weight = s_wavePathPoint( _to.x, _to.y, p_to1 );
		p_to2->weight = s_wavePathPoint( _to.x, _to.y, p_to2 );
				
		Poly2Tri::Triangle * n0 = tr_to->GetNeighbor( 0 );
		Poly2Tri::Triangle * n1 = tr_to->GetNeighbor( 1 );
		Poly2Tri::Triangle * n2 = tr_to->GetNeighbor( 2 );

		s_wavePathTriangle( n0 );
		s_wavePathTriangle( n1 );
		s_wavePathTriangle( n2 );

		TVectorPathPoint path;

		s_slitherPath( nullptr, tr_from, tr_to, path );
		
		return true;
	}
}