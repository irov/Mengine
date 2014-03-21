#	include "PathFinderMap.h"
#	include "PathFinderWay.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap::PathFinderMap( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
		, m_obstacleEnumerator(0)
		, m_cachePointUse(0)
		, m_cachePointUse2(0)
		, m_unitSize(20.f)
		, m_sweepContext(nullptr)
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

		Polygon::ring_type & ring = _big.outer();

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

			mt::vec2f & p_new = ring[i];
			p_new += d_norm;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PathFinderMap::addObstacle( const Polygon & _polygon )
	{		
		Polygon big_polygon = _polygon;
		boost::geometry::correct( big_polygon );

		s_enlargePolygonFromLow( big_polygon, m_unitSize );		

		if( this->testBigHolesPolygon_( big_polygon ) == true )
		{
			return 0;
		}

		size_t id = ++m_obstacleEnumerator;

		Obstacle obstacle;
		obstacle.id = id;

		obstacle.hole = _polygon;
		boost::geometry::correct( obstacle.hole );

		obstacle.bigHole = big_polygon;

		m_obstales.push_back( obstacle );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::removeObstacle( size_t _id )
	{
		for( TObstacles::iterator
			it = m_obstales.begin(),
			it_end = m_obstales.end();
		it != it_end;
		++it )
		{
			if( it->id != _id )
			{
				continue;
			}

			m_obstales.erase( it );

			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testHolesPolygon_( const Polygon & _polygon ) const
	{
		for( TObstacles::const_iterator
			it = m_obstales.begin(),
			it_end = m_obstales.end();
		it != it_end;
		++it )
		{
			const Polygon & p = it->hole;

			bool intersect = boost::geometry::intersects( p, _polygon );

			if( intersect == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testBigHolesPolygon_( const Polygon & _polygon ) const
	{
		for( TObstacles::const_iterator
			it = m_obstales.begin(),
			it_end = m_obstales.end();
		it != it_end;
		++it )
		{
			const Polygon & p = it->hole;

			bool intersect = boost::geometry::intersects( p, _polygon );

			if( intersect == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	PFMPoint * PathFinderMap::createPoint_( float _x, float _y )
	{
		if( m_cachePointUse >= 1024 )
		{
			return nullptr;
		}

		PFMPoint & p = m_cachePoint[m_cachePointUse];
		++m_cachePointUse;

		p.x = _x;
		p.y = _y;

		p.edge_list.clear();
		p.weight = 0.f;
		p.neighbor.clear();

		return &p;
	}
	//////////////////////////////////////////////////////////////////////////
	PFMPoint * PathFinderMap::createPoint2_( float _x, float _y )
	{
		if( m_cachePointUse2 >= 1024 )
		{
			return nullptr;
		}

		PFMPoint & p = m_cachePoint2[m_cachePointUse2];
		++m_cachePointUse2;

		p.x = _x;
		p.y = _y;

		p.edge_list.clear();
		p.weight = 0.f;
		p.neighbor.clear();

		return &p;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::makePolyPointFromPolygon_( const Polygon & _polygon, Points & _points )
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

			PFMPoint * p = this->createPoint_( v.x, v.y );

			if( p == nullptr )
			{
				return false;
			}

			_points.push_back( p );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_pointAddNeighbor( PFMPoint * p, PFMPoint * n )
	{
		p->neighbor.push_back( n );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_findOutPoint( p2t::Triangle * _tr0, p2t::Triangle * _tr1, PFMPoint ** _p0, PFMPoint ** _p1 )
	{
		PFMPoint * p00 = (PFMPoint *)_tr0->GetPoint(0);
		PFMPoint * p01 = (PFMPoint *)_tr0->GetPoint(1);
		PFMPoint * p02 = (PFMPoint *)_tr0->GetPoint(2);

		if( _tr1->Contains( p00 ) == false )
		{
			*_p0 = p00;
		}
		else if( _tr1->Contains( p01 ) == false )
		{
			*_p0 = p01;
		}
		else if( _tr1->Contains( p02 ) == false )
		{
			*_p0 = p02;
		}
		
		PFMPoint * p10 = (PFMPoint *)_tr1->GetPoint(0);
		PFMPoint * p11 = (PFMPoint *)_tr1->GetPoint(1);
		PFMPoint * p12 = (PFMPoint *)_tr1->GetPoint(2);

		if( _tr0->Contains( p10 ) == false )
		{
			*_p1 = p10;
		}
		else if( _tr0->Contains( p11 ) == false )
		{
			*_p1 = p11;
		}
		else if( _tr0->Contains( p12 ) == false )
		{
			*_p1 = p12;
		}
	}
	//////////////////////////////////////////////////////////////////////////	
	void PathFinderMap::attachNeighbor_( Points & _points, Points::size_type _begin, Points::size_type _end, PFMPoint * _p0 ) const
	{
		mt::vec2f v0(_p0->x, _p0->y);

		for( Points::size_type
			it_next = _begin,
			it_next_end = _end;
		it_next != it_next_end;
		++it_next )
		{
			PFMPoint * p1 = (PFMPoint *)_points[it_next];

			mt::vec2f v1(p1->x, p1->y);

			if( this->testHolesSegment_( v0, v1 ) == true )
			{
				continue;
			}

			s_pointAddNeighbor( _p0, p1 );
			s_pointAddNeighbor( p1, _p0 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::generateMap()
	{
		m_points.clear();

		for( TObstacles::const_iterator
			it = m_obstales.begin(),
			it_end = m_obstales.end();
		it != it_end;
		++it )
		{
			const Polygon & p = it->bigHole;

			this->makePolyPointFromPolygon_( p, m_points );
		}

		Points::size_type points_size = m_points.size();

		for( Points::size_type
			it = 0,
			it_end = points_size - 1;
		it != it_end;
		++it )
		{
			PFMPoint * p0 = (PFMPoint *)m_points[it];

			this->attachNeighbor_( m_points, it + 1, points_size, p0 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::generateMap2()
	{
		m_cachePointUse = 0;

		Points mapPoints;
		this->makePolyPointFromPolygon_( m_mapPolygon, mapPoints );
		
		m_sweepContext = new p2t::SweepContext( mapPoints );

		for( TObstacles::const_iterator
			it = m_obstales.begin(),
			it_end = m_obstales.end();
		it != it_end;
		++it )
		{
			const Polygon & p = it->bigHole;

			Points holePoint;
			this->makePolyPointFromPolygon_( p, holePoint );

			m_sweepContext->AddHole( holePoint );
		}

		p2t::Sweep sweep;
		sweep.Triangulate( *m_sweepContext );

		Triangles & triangles = m_sweepContext->GetTriangles();

		for( Triangles::iterator
			it = triangles.begin(),
			it_end = triangles.end();
		it != it_end;
		++it )
		{
			p2t::Triangle * tr = *it;

			if( tr->IsInterior() == false )
			{
				continue;
			}

			PFMPoint * p0 = (PFMPoint *)tr->GetPoint(0);
			PFMPoint * p1 = (PFMPoint *)tr->GetPoint(1);
			PFMPoint * p2 = (PFMPoint *)tr->GetPoint(2);

			s_pointAddNeighbor( p0, p1 );
			s_pointAddNeighbor( p0, p2 );
			s_pointAddNeighbor( p1, p0 );
			s_pointAddNeighbor( p1, p2 );
			s_pointAddNeighbor( p2, p0 );
			s_pointAddNeighbor( p2, p1 );
		}

		for( Triangles::iterator
			it = triangles.begin(),
			it_end = triangles.end();
		it != it_end;
		++it )
		{
			p2t::Triangle * tr = *it;

			if( tr->IsInterior() == false )
			{
				continue;
			}

			p2t::Triangle * tr0 = tr->GetNeighbor(0);

			if( tr0 != nullptr && tr0->IsInterior() == true )
			{
				PFMPoint * p0;
				PFMPoint * p1;

				s_findOutPoint( tr, tr0, &p0, &p1 );

				s_pointAddNeighbor( p0, p1 );
			}

			p2t::Triangle * tr1 = tr->GetNeighbor(1);

			if( tr1 != nullptr && tr1->IsInterior() == true )
			{
				PFMPoint * p0;
				PFMPoint * p1;

				s_findOutPoint( tr, tr1, &p0, &p1 );

				s_pointAddNeighbor( p0, p1 );
			}

			p2t::Triangle * tr2 = tr->GetNeighbor(2);

			if( tr2 != nullptr && tr2->IsInterior() == true )
			{
				PFMPoint * p0;
				PFMPoint * p1;

				s_findOutPoint( tr, tr2, &p0, &p1 );

				s_pointAddNeighbor( p0, p1 );
			}
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_existPointInTriangle( p2t::Triangle * _tr, const mt::vec2f & _point )
	{
		p2t::Point * p0 = _tr->GetPoint(0);
		p2t::Point * p1 = _tr->GetPoint(1);
		p2t::Point * p2 = _tr->GetPoint(2);

		mt::vec2f A(p0->x, p0->y);
		mt::vec2f B(p1->x, p1->y);
		mt::vec2f C(p2->x, p2->y);
		mt::vec2f P(_point);

		mt::vec2f v0 = C - A;
		mt::vec2f v1 = B - A;
		mt::vec2f v2 = P - A;

		float dot00 = mt::dot_v2_v2(v0, v0);
		float dot01 = mt::dot_v2_v2(v0, v1);
		float dot02 = mt::dot_v2_v2(v0, v2);
		float dot11 = mt::dot_v2_v2(v1, v1);
		float dot12 = mt::dot_v2_v2(v1, v2);

		float invDenom = 1.f / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return (u >= -0.001f) && (v >= -0.001f) && (u + v <= 1.001f);
	}
	//////////////////////////////////////////////////////////////////////////
	static p2t::Triangle * s_findTriangle( Triangles & _triangles, const mt::vec2f & _point )
	{
		for( Triangles::iterator
			it = _triangles.begin(),
			it_end = _triangles.end();
		it != it_end;
		++it )
		{
			p2t::Triangle * tr = *it;

			if( s_existPointInTriangle( tr, _point ) == false )
			{
				continue;
			}

			return tr;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static float s_lengthPoint( float _x, float _y, p2t::Point * _point )
	{
		float wavelength = sqrtf( (_point->x - _x) * (_point->x - _x) + (_point->y - _y) * (_point->y - _y) );

		return wavelength;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wavePathPoint( PFMPoint * _point )
	{	
		float x = _point->x;
		float y = _point->y;
		float weight = _point->weight;

		for( PFMPoint::TVectorNeighbor::iterator
			it = _point->neighbor.begin(),
			it_end = _point->neighbor.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			float length = s_lengthPoint( x, y, neighbor );
			float neighbor_weight = weight + length;


			if( neighbor->weight <= neighbor_weight )
			{
				continue;
			}

			neighbor->weight = neighbor_weight;

			s_wavePathPoint( neighbor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_slitherPath3( PFMPoint * _minimal, PFMPoint * _p0, PFMPoint * _p1, PFMPoint * _p2, PFMPoints & _points )
	{
		PFMPoint * minimalNeighbor = _minimal;

		_points.push_back( minimalNeighbor );

		if( minimalNeighbor == _p0 || minimalNeighbor == _p1 || minimalNeighbor == _p2 )
		{
			return;
		}

		for( PFMPoint::TVectorNeighbor::iterator
			it = _minimal->neighbor.begin(),
			it_end = _minimal->neighbor.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			if( neighbor->weight > minimalNeighbor->weight )
			{
				continue;
			}

			minimalNeighbor = neighbor;
		}

		if( minimalNeighbor == _minimal )
		{
			return;
		}

		s_slitherPath3( minimalNeighbor, _p0, _p1, _p2, _points );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_slitherPath1( PFMPoint * _minimal, PFMPoint * _p0, PFMPoints & _points )
	{
		PFMPoint * minimalNeighbor = _minimal;

		_points.push_back( minimalNeighbor );

		if( minimalNeighbor == _p0 )
		{
			return;
		}

		for( PFMPoint::TVectorNeighbor::iterator
			it = _minimal->neighbor.begin(),
			it_end = _minimal->neighbor.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			if( neighbor->weight > minimalNeighbor->weight )
			{
				continue;
			}

			minimalNeighbor = neighbor;
		}

		if( minimalNeighbor == _minimal )
		{
			return;
		}

		s_slitherPath1( minimalNeighbor, _p0, _points );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const
	{
		mt::vec2f dir;
		mt::norm_v2( dir, _p1 - _p0 );

		mt::vec2f perp;
		mt::perp_v2( perp, dir );
		
		Polygon way_polygon;
		boost::geometry::append( way_polygon, _p0 + perp );
		boost::geometry::append( way_polygon, _p1 + perp );
		boost::geometry::append( way_polygon, _p1 - perp );
		boost::geometry::append( way_polygon, _p0 - perp );
		boost::geometry::correct( way_polygon );

		bool test = this->testHolesPolygon_( way_polygon );

		return test;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_addNeignborWayPoint( PFMPoint * _current, PFMPoint * _next )
	{
		float length = s_lengthPoint( _current->x, _current->y, _next );

		if( _next->weight <= _current->weight - length )
		{
			return false;
		}

		_next->weight = _current->weight + length;

		_current->neighbor.push_back( _next );
		_next->neighbor.push_back( _current );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::calcNeighborPoints_( PFMPoints & _wayPoints ) const
	{
		PFMPoints::size_type point_size = _wayPoints.size();

		for( PFMPoints::size_type 
			it_current = 0,
			it_current_end = point_size - 1;
		it_current != it_current_end;
		++it_current )
		{
			PFMPoint * point_current = _wayPoints[it_current];

			PFMPoints::size_type it_next = it_current + 1;
			PFMPoint * point_next = _wayPoints[it_next];

			if( s_addNeignborWayPoint( point_current, point_next ) == false )
			{
				continue;
			}
		
			for( PFMPoints::size_type 
				it_forward = it_next + 1,
				it_forward_end = point_size; 
			it_forward != it_forward_end; 
			++it_forward )
			{	
				PFMPoint * point_forward = _wayPoints[it_forward];

				mt::vec2f p0(point_current->x, point_current->y);
				mt::vec2f p1(point_forward->x, point_forward->y);

				if( this->testHolesSegment_( p0, p1 ) == true )
				{
					break;
				}

				s_addNeignborWayPoint( point_current, point_forward );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static PFMPoint * s_minimalWeightPoint( PFMPoint * _p0, PFMPoint * _p1, PFMPoint * _p2 )
	{
		if( _p0->weight <= _p1->weight && _p0->weight <= _p2->weight )
		{
			return _p0;
		}

		if( _p1->weight <= _p2->weight && _p1->weight <= _p0->weight )
		{
			return _p1;
		}

		if( _p2->weight <= _p0->weight && _p2->weight <= _p1->weight )
		{
			return _p2;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	PFMPoint * PathFinderMap::findNearestPoint_( const mt::vec2f & _v ) const
	{
		PFMPoint * minimal_point = nullptr;
		float minimal_length = 1024.f * 1024.f;

		for( Points::const_iterator
			it = m_points.begin(),
			it_end = m_points.end();
		it != it_end;
		++it )
		{
			PFMPoint * p = (PFMPoint *)*it;

			mt::vec2f pv(p->x, p->y);

			float length = mt::length_v2_v2( _v, pv );
			
			if( length < minimal_length )
			{
				minimal_length = length;
				minimal_point = p;
			}
		}

		return minimal_point;
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay * PathFinderMap::findPath( const mt::vec2f & _from, const mt::vec2f & _to )
	{
		if( this->testHolesSegment_( _from, _to ) == false )
		{
			TVectorWayPoint wayPoints;

			wayPoints.push_back( _from );
			wayPoints.push_back( _to );

			PathFinderWay * way = new PathFinderWay(m_serviceProvider);

			way->initialize( _from, _to, wayPoints );

			m_pathFinderWays.push_back( way );

			return way;
		}


		PFMPoint * point_from = createPoint2_( _from.x, _from.y );
		PFMPoint * point_to = createPoint2_( _to.x, _to.y );

		Points::size_type points_size = m_points.size();

		this->attachNeighbor_( m_points, 0, points_size, point_from );
		this->attachNeighbor_( m_points, 0, points_size, point_to );


		//PFMPoint * p = this->findNearestPoint_()

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay * PathFinderMap::findPath2( const mt::vec2f & _from, const mt::vec2f & _to )
	{
		if( this->testHolesSegment_( _from, _to ) == false )
		{
			TVectorWayPoint wayPoints;

			wayPoints.push_back( _from );
			wayPoints.push_back( _to );

			PathFinderWay * way = new PathFinderWay(m_serviceProvider);

			way->initialize( _from, _to, wayPoints );

			m_pathFinderWays.push_back( way );

			return way;
		}

		Triangles & triangles = m_sweepContext->GetTriangles();
		
		p2t::Triangle * tr_from = s_findTriangle( triangles, _from );
		p2t::Triangle * tr_to = s_findTriangle( triangles, _to );

		if( tr_to == nullptr || tr_to->IsInterior() == false || tr_from == nullptr || tr_from->IsInterior() == false )
		{
			return nullptr;
		}
		
		for( size_t i = 0; i != m_cachePointUse; ++i )
		{
			m_cachePoint[i].weight = 1024.f * 1024.f;			
		}
		
		PFMPoint * p_to0 = (PFMPoint *)tr_to->GetPoint( 0 );
		PFMPoint * p_to1 = (PFMPoint *)tr_to->GetPoint( 1 );
		PFMPoint * p_to2 = (PFMPoint *)tr_to->GetPoint( 2 );

		p_to0->weight = s_lengthPoint( _to.x, _to.y, p_to0 );
		p_to1->weight = s_lengthPoint( _to.x, _to.y, p_to1 );
		p_to2->weight = s_lengthPoint( _to.x, _to.y, p_to2 );

		s_wavePathPoint( p_to0 );
		s_wavePathPoint( p_to1 );
		s_wavePathPoint( p_to2 );

		PFMPoint * p_from0 = (PFMPoint *)tr_from->GetPoint( 0 );
		PFMPoint * p_from1 = (PFMPoint *)tr_from->GetPoint( 1 );
		PFMPoint * p_from2 = (PFMPoint *)tr_from->GetPoint( 2 );
	
		PFMPoint * minimalPoint = s_minimalWeightPoint( p_from0, p_from1, p_from2 );

		PFMPoints wayPoints;				
		s_slitherPath3( minimalPoint, p_to0, p_to1, p_to2, wayPoints );
		
		m_cachePointUse2 = 0;

		PFMPoints wayPoints2;
		PFMPoint * p_from = this->createPoint2_( _from.x, _from.y );
		PFMPoint * p_to = this->createPoint2_( _to.x, _to.y );

		wayPoints2.push_back( p_from );

		for( PFMPoints::const_iterator
			it = wayPoints.begin(),
			it_end = wayPoints.end();
		it != it_end;
		++it )
		{
			const PFMPoint * p = *it;

			PFMPoint * p2 = this->createPoint2_( p->x, p->y );

			wayPoints2.push_back( p2 );
		}

		wayPoints2.push_back( p_to );

		for( PFMPoints::iterator
			it = wayPoints2.begin(),
			it_end = wayPoints2.end();
		it != it_end;
		++it )
		{
			PFMPoint * p = *it;

			p->weight = 1024.f * 1024.f;
		}

		p_from->weight = 0.f;
		
		this->calcNeighborPoints_( wayPoints2 );

		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			);

		for( PFMPoints::const_iterator
			it = wayPoints2.begin(),
			it_end = wayPoints2.end();
		it != it_end;
		++it )
		{
			const PFMPoint * point = *it;

			printf("point %.0f:%.0f to"
				, point->x
				, point->y
				);

			for( PFMPoint::TVectorNeighbor::const_iterator
				it_neigbor = point->neighbor.begin(),
				it_neigbor_end = point->neighbor.end();
			it_neigbor != it_neigbor_end;
			++it_neigbor )
			{
				const PFMPoint * n = *it_neigbor;

				printf(" %.0f:%0.f"
					, n->x
					, n->y
					);
			}

			printf("\n");
		}

		PFMPoint * first = wayPoints2.front();
		PFMPoint * last = wayPoints2.back();

		PFMPoints wayPoints3;
		s_slitherPath1( last, first, wayPoints3 );

		//TVectorWayPoint wayPointsFilters2;
		//this->filterWayPoints_( wayPointsFilters2, wayPoints );

		//TVectorWayPoint wayPointsFilters1;
		//this->filterWayPoints_( wayPointsFilters1, wayPointsFilters2 );

		//TVectorWayPoint wayPointsFilters;
		//this->filterWayPoints_( wayPointsFilters, wayPointsFilters1 );

		TVectorWayPoint wayPoints4;

		for( PFMPoints::const_reverse_iterator
			it = wayPoints3.rbegin(),
			it_end = wayPoints3.rend();
		it != it_end;
		++it )
		{
			const PFMPoint * point = *it;

			wayPoints4.push_back( mt::vec2f(point->x, point->y) );
		}
		
		PathFinderWay * way = new PathFinderWay(m_serviceProvider);

		way->initialize( _from, _to, wayPoints4 );
		
		m_pathFinderWays.push_back( way );

		return way;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::removePath( PathFinderWay * _way )
	{
		TVectorPathFinderWay::iterator it_found = std::find( m_pathFinderWays.begin(), m_pathFinderWays.end(), _way );

		if( it_found == m_pathFinderWays.end() )
		{
			return;
		}

		delete *it_found;

		m_pathFinderWays.erase( it_found );		
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setCamera2D( const RenderCameraInterface * _camera )
	{
		m_camera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) == 0 )
		{
			return;
		}

		if( m_camera != nullptr )
		{
			_camera = m_camera;
		}

		if( m_sweepContext != nullptr )
		{
			const Triangles & triangles = m_sweepContext->GetTriangles();

			for( Triangles::const_iterator
				it = triangles.begin(),
				it_end = triangles.end();
			it != it_end;
			++it )
			{
				p2t::Triangle * tr = *it;

				const p2t::Point * p0 = tr->GetPoint(0);
				const p2t::Point * p1 = tr->GetPoint(1);
				const p2t::Point * p2 = tr->GetPoint(2);

				RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
					->getDebugRenderVertex2D( 3 * 2 );

				vertices[0].pos.x = p0->x;
				vertices[0].pos.y = p0->y;
				vertices[0].pos.z = 0.f;

				vertices[1].pos.x = p1->x;
				vertices[1].pos.y = p1->y;
				vertices[1].pos.z = 0.f;


				vertices[2].pos.x = p1->x;
				vertices[2].pos.y = p1->y;
				vertices[2].pos.z = 0.f;

				vertices[3].pos.x = p2->x;
				vertices[3].pos.y = p2->y;
				vertices[3].pos.z = 0.f;


				vertices[4].pos.x = p2->x;
				vertices[4].pos.y = p2->y;
				vertices[4].pos.z = 0.f;

				vertices[5].pos.x = p0->x;
				vertices[5].pos.y = p0->y;
				vertices[5].pos.z = 0.f;

				for( size_t i = 0; i != 6; ++i )
				{
					vertices[i].color = 0xffffffff;
					vertices[i].uv.x = 0.f;
					vertices[i].uv.y = 0.f;
					vertices[i].uv2.x = 0.f;
					vertices[i].uv2.y = 0.f;
				}

				const RenderMaterialInterfacePtr & material = RENDER_SERVICE(m_serviceProvider)
					->getDebugMaterial();

				RENDER_SERVICE(m_serviceProvider)
					->addRenderLine( _viewport, _camera, material, vertices, 6, nullptr );
			}
		}

		for( Points::const_iterator
			it = m_points.begin(),
			it_end = m_points.end();
		it != it_end;
		++it )
		{
			PFMPoint * p = (PFMPoint*)*it;
			
			size_t numpoints = p->neighbor.size();

			if( numpoints == 0 )
			{
				continue;
			}

			size_t vertexCount = numpoints * 2;

			RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
				->getDebugRenderVertex2D( vertexCount );

			if( vertices == nullptr )
			{
				return;
			}

			for( PFMPoint::TVectorNeighbor::size_type
				it_neighbor = 0,
				it_neighbor_end = p->neighbor.size();
			it_neighbor != it_neighbor_end;
			++it_neighbor )
			{		
				PFMPoint * n = (PFMPoint*)p->neighbor[it_neighbor];

				RenderVertex2D & v0 = vertices[it_neighbor*2+0];

				v0.pos.x = p->x;
				v0.pos.y = p->y;
				v0.pos.z = 0.f;

				v0.color = 0xFF00FFFF;
				v0.uv.x = 0.f;
				v0.uv.y = 0.f;
				v0.uv2.x = 0.f;
				v0.uv2.y = 0.f;

				RenderVertex2D & v1 = vertices[it_neighbor*2+1];

				v1.pos.x = n->x;
				v1.pos.y = n->y;
				v1.pos.z = 0.f;

				v1.color = 0xFF00FFFF;
				v1.uv.x = 0.f;
				v1.uv.y = 0.f;
				v1.uv2.x = 0.f;
				v1.uv2.y = 0.f;
			}

			const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
				->getDebugMaterial();

			RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
				, vertices
				, vertexCount
				, nullptr
				);
		}

		for( TObstacles::iterator
			it = m_obstales.begin(),
			it_end = m_obstales.end();
		it != it_end;
		++it )
		{
			const Polygon & polygon = it->hole;

			size_t numpoints = boost::geometry::num_points(polygon);

			if( numpoints == 0 )
			{
				continue;
			}

			size_t vertexCount = numpoints * 2;

			RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
				->getDebugRenderVertex2D( vertexCount );

			if( vertices == nullptr )
			{
				return;
			}

			const Polygon::ring_type & ring = polygon.outer();

			for( size_t i = 0; i != numpoints; ++i )
			{
				size_t j = (i + 1) % numpoints;

				mt::vec2f trP0 = ring[i];
				RenderVertex2D & v0 = vertices[i*2+0];

				v0.pos.x = trP0.x;
				v0.pos.y = trP0.y;
				v0.pos.z = 0.f;

				v0.color = 0xFF00FFFF;
				v0.uv.x = 0.f;
				v0.uv.y = 0.f;
				v0.uv2.x = 0.f;
				v0.uv2.y = 0.f;

				mt::vec2f trP1 = ring[j];
				RenderVertex2D & v1 = vertices[i*2+1];

				v1.pos.x = trP1.x;
				v1.pos.y = trP1.y;
				v1.pos.z = 0.f;

				v1.color = 0xFF00FFFF;
				v1.uv.x = 0.f;
				v1.uv.y = 0.f;
				v1.uv2.x = 0.f;
				v1.uv2.y = 0.f;
			}

			const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
				->getDebugMaterial();

			RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
				, vertices
				, vertexCount
				, nullptr
				);
		}

		for( TVectorPathFinderWay::iterator
			it = m_pathFinderWays.begin(),
			it_end = m_pathFinderWays.end();
		it != it_end;
		++it )
		{
			PathFinderWay * way = *it;

			way->render( _viewport, _camera );
		}
	}
}