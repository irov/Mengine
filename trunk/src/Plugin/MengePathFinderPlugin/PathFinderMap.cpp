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
	PFMPoint * PathFinderMap::createPoint_( const mt::vec2f & _v )
	{
		if( m_cachePointUse >= 1024 )
		{
			return nullptr;
		}

		PFMPoint & p = m_cachePoint[m_cachePointUse];
		++m_cachePointUse;

		p.v = _v;

		p.weight = 0.f;
		p.neighbor.clear();

		return &p;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::makePolyPointFromPolygon_( const Polygon & _polygon, PFMPoints & _points )
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

			PFMPoint * p = this->createPoint_( v );

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
	void PathFinderMap::attachNeighbor_( PFMPoints & _points, PFMPoints::size_type _begin, PFMPoints::size_type _end, PFMPoint * _p0 ) const
	{
		for( PFMPoints::size_type
			it_next = _begin,
			it_next_end = _end;
		it_next != it_next_end;
		++it_next )
		{
			PFMPoint * p1 = _points[it_next];

			if( this->testHolesSegment_( _p0->v, p1->v ) == true )
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

		PFMPoints::size_type points_size = m_points.size();

		for( PFMPoints::size_type
			it = 0,
			it_end = points_size - 1;
		it != it_end;
		++it )
		{
			PFMPoint * p0 = m_points[it];

			this->attachNeighbor_( m_points, it + 1, points_size, p0 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wavePathPoint( PFMPoint * _point )
	{	
		float weight = _point->weight;

		for( PFMPoint::TVectorNeighbor::iterator
			it = _point->neighbor.begin(),
			it_end = _point->neighbor.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			float length = mt::length_v2_v2( _point->v, neighbor->v );
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
		float length = mt::length_v2_v2( _current->v, _next->v );

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
				
				if( this->testHolesSegment_( point_current->v, point_forward->v ) == true )
				{
					break;
				}

				s_addNeignborWayPoint( point_current, point_forward );
			}
		}
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


		PFMPoint point_from(_from);
		PFMPoint point_to(_to);

		PFMPoints::size_type points_size = m_points.size();

		this->attachNeighbor_( m_points, 0, points_size, &point_from );
		this->attachNeighbor_( m_points, 0, points_size, &point_to );
		
		PathFinderWay * way = new PathFinderWay(m_serviceProvider);

		//way->initialize( _from, _to, wayPoints4 );

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

		for( PFMPoints::const_iterator
			it = m_points.begin(),
			it_end = m_points.end();
		it != it_end;
		++it )
		{
			PFMPoint * p = *it;
			
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
				PFMPoint * n = p->neighbor[it_neighbor];

				RenderVertex2D & v0 = vertices[it_neighbor*2+0];

				v0.pos.x = p->v.x;
				v0.pos.y = p->v.y;
				v0.pos.z = 0.f;

				v0.color = 0xFF00FFFF;
				v0.uv.x = 0.f;
				v0.uv.y = 0.f;
				v0.uv2.x = 0.f;
				v0.uv2.y = 0.f;

				RenderVertex2D & v1 = vertices[it_neighbor*2+1];

				v1.pos.x = n->v.x;
				v1.pos.y = n->v.y;
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