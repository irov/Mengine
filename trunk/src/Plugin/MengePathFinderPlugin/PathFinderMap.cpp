#	include "PathFinderMap.h"
#	include "PathFinderWay.h"

#	include "Interface/WatchdogInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_pointAddNeighbor( PFMPoint * p, PFMPoint * n )
	{
		p->neighbor_other.push_back( n );
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap::PathFinderMap( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
		, m_obstacleEnumerator(0)
		, m_cachePointUse(0)
		, m_cachePointUse2(0)
		, m_unitSize(20.f)
		, m_invalidateMap(true)
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
	static void s_enlargePolygonRingFromLow( Polygon::ring_type & _ring, float _radius )
	{
		if( _ring.empty() == true )
		{
			return;
		}

		size_t numpoints = _ring.size();

		--numpoints;

		for( size_t i = 0; i != numpoints; ++i )
		{
			size_t i0 = (i + numpoints - 1) % numpoints;
			size_t i1 = (i + numpoints + 0) % numpoints;
			size_t i2 = (i + numpoints + 1) % numpoints;

			const mt::vec2f & p0 = _ring[i0];
			const mt::vec2f & p1 = _ring[i1];
			const mt::vec2f & p2 = _ring[i2];

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

			mt::vec2f & p_new = _ring[i];
			p_new += d_norm;
		}

		_ring[numpoints] = _ring[0];
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_reducePolygonRingFromLow( Polygon::ring_type & _ring, float _radius )
	{
		if( _ring.empty() == true )
		{
			return;
		}

		size_t numpoints = _ring.size();

		--numpoints;

		for( size_t i = 0; i != numpoints; ++i )
		{
			size_t i0 = (i + numpoints - 1) % numpoints;
			size_t i1 = (i + numpoints + 0) % numpoints;
			size_t i2 = (i + numpoints + 1) % numpoints;
			
			const mt::vec2f & p0 = _ring[i0];
			const mt::vec2f & p1 = _ring[i1];
			const mt::vec2f & p2 = _ring[i2];

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

			mt::vec2f & p_new = _ring[i];
			p_new += d_norm;
		}

		_ring[numpoints] = _ring[0];
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_enlargePolygonFromLow( Polygon & _big, float _radius )
	{
		Polygon::ring_type & ring = _big.outer();

		s_enlargePolygonRingFromLow( ring, _radius );

		Polygon::inner_container_type & inners = _big.inners();

		for( Polygon::inner_container_type::iterator
			it = inners.begin(),
			it_end = inners.end();
		it != it_end;
		++it )
		{
			Polygon::ring_type & ring = *it;

			s_reducePolygonRingFromLow( ring, _radius );
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

		Obstacle * obstacle = new Obstacle;
		obstacle->id = id;

		obstacle->hole = _polygon;
		boost::geometry::correct( obstacle->hole );

		obstacle->bigHole = big_polygon;

		this->makePolyPointFromPolygon_( obstacle->bigHole, obstacle->points );
		
		PFMPoints::size_type point_size = obstacle->points.size();

		for( PFMPoints::size_type 
			it = 0,
			it_end = point_size;
		it != it_end;
		++it )
		{
			PFMPoints::size_type i0 = (it + point_size - 1) % point_size;
			PFMPoints::size_type i1 = (it + point_size + 0) % point_size;
			PFMPoints::size_type i2 = (it + point_size + 1) % point_size;

			PFMPoint * p0 = obstacle->points[i0];
			PFMPoint * p1 = obstacle->points[i1];
			PFMPoint * p2 = obstacle->points[i2];

			p1->neighbor_left = p0;
			p1->neighbor_right = p2;
			p0->neighbor_right = p1;
			p2->neighbor_left = p1;						
		}
					
		m_obstacles.push_back( obstacle );

		m_invalidateMap = true;

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::removeObstacle( size_t _id )
	{
		for( TVectorObstacles::iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			if( (*it)->id != _id )
			{
				continue;
			}

			m_obstacles.erase( it );

			break;
		}

		m_invalidateMap = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testHolesPolygon_( const Polygon & _polygon ) const
	{
		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & p = (*it)->hole;

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
		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & p = (*it)->hole;

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
		p.neighbor_other.clear();
		p.neighbor_left = nullptr;
		p.neighbor_right = nullptr;

		return &p;
	}
	//////////////////////////////////////////////////////////////////////////
	PFMPoint * PathFinderMap::createPoint2_( const mt::vec2f & _v )
	{
		if( m_cachePointUse2 >= 1024 )
		{
			return nullptr;
		}

		PFMPoint & p = m_cachePoint2[m_cachePointUse2];
		++m_cachePointUse2;

		p.v = _v;

		p.weight = 0.f;
		p.neighbor_other.clear();
		p.neighbor_left = nullptr;
		p.neighbor_right = nullptr;

		return &p;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::makePolyPointFromRing_( const Polygon::ring_type & _ring, PFMPoints & _points )
	{
		if( _ring.empty() == true )
		{
			return true;
		}
		
		size_t numpoints = _ring.size();

		--numpoints;

		for( size_t
			it = 0,
			it_end = numpoints;
		it != it_end;
		++it )
		{
			const mt::vec2f & v = _ring[it];

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
	bool PathFinderMap::makePolyPointFromPolygon_( const Polygon & _polygon, PFMPoints & _points )
	{
		const Polygon::inner_container_type & inner_container = _polygon.inners();

		for( Polygon::inner_container_type::const_iterator 
			it = inner_container.begin(),
			it_end = inner_container.end();
		it != it_end;
		++it )
		{
			const Polygon::ring_type & ring = *it;

			if( this->makePolyPointFromRing_( ring, _points ) == false )
			{
				return false;
			}
		}

		if( inner_container.empty() == false )
		{
			return false;
		}

		const Polygon::ring_type & ring = _polygon.outer();

		if( this->makePolyPointFromRing_( ring, _points ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////	
	void PathFinderMap::attachNeighbor2_( TVectorObstacles::size_type _begin, TVectorObstacles::size_type _end, PFMPoint * _point )
	{
		for( TVectorObstacles::size_type
			it = _begin,
			it_end = _end;
		it != it_end;
		++it )
		{
			Obstacle * obstacle = m_obstacles[it];

			for( PFMPoints::iterator 
				it_point = obstacle->points.begin(),
				it_point_end = obstacle->points.end();
			it_point != it_point_end;
			++it_point )
			{
				PFMPoint * point = *it_point;

				if( this->testHolesSegment_( point->v, _point->v ) == true )
				{
					continue;
				}

				s_pointAddNeighbor( point, _point );
				s_pointAddNeighbor( _point, point );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::attachNeighbor_( PFMPoint * _point )
	{
		TVectorObstacles::size_type size = m_obstacles.size();

		this->attachNeighbor2_( 0, size, _point );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::generateMap()
	{
		for( TVectorObstacles::iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			Obstacle * obstacle = *it;

			for( PFMPoints::iterator 
				it_point = obstacle->points.begin(),
				it_point_end = obstacle->points.end();
			it_point != it_point_end;
			++it_point )
			{
				PFMPoint * point = *it_point;

				point->neighbor_other.clear();
			}
		}

		TVectorObstacles::size_type obstacle_count = m_obstacles.size();

		for( TVectorObstacles::size_type
			it = 0,
			it_end = obstacle_count - 1;
		it != it_end;
		++it )
		{
			Obstacle * obstacle = m_obstacles[it];

			for( PFMPoints::iterator
				it_point = obstacle->points.begin(),
				it_point_end = obstacle->points.end();
			it_point != it_point_end;
			++it_point )
			{
				PFMPoint * point = *it_point;

				this->attachNeighbor2_( it + 1, obstacle_count, point );
			}
		}

		m_invalidateMap = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wavePathPoint3( PFMPoint * _point, PFMPoint * _neighbor, PFMPoints & _nexts )
	{
		float weight = _point->weight;

		float length = mt::length_v2_v2( _point->v, _neighbor->v );
		float neighbor_weight = weight + length;

		if( _neighbor->weight <= neighbor_weight )
		{
			return;
		}

		_neighbor->weight = neighbor_weight;

		_nexts.push_back( _neighbor );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wavePathPoint2( PFMPoint * _point, PFMPoints & _nexts )
	{
		float weight = _point->weight;
		
		for( PFMPoint::TVectorNeighbor::iterator
			it = _point->neighbor_other.begin(),
			it_end = _point->neighbor_other.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			s_wavePathPoint3( _point, neighbor, _nexts );
		}

		if( _point->neighbor_left != nullptr )
		{
			s_wavePathPoint3( _point, _point->neighbor_left, _nexts );
		}

		if( _point->neighbor_right != nullptr )
		{
			s_wavePathPoint3( _point, _point->neighbor_right, _nexts );		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wavePathPoint( PFMPoint * _point )
	{	
		PFMPoints nexts;
		s_wavePathPoint2( _point, nexts );

		while( nexts.empty() == false )
		{
			PFMPoints nexts2;
			for( PFMPoints::iterator
				it = nexts.begin(),
				it_end = nexts.end();
			it != it_end;
			++it )
			{
				PFMPoint * neighbor = *it;

				s_wavePathPoint2( neighbor, nexts2 );
			}

			nexts = nexts2;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_slitherPath2( PFMPoint * _minimal, PFMPoint * _neighbor )
	{
		float length = mt::length_v2_v2( _minimal->v, _neighbor->v );
		float neighbor_weight = _minimal->weight - length;

		if( fabsf( _neighbor->weight - neighbor_weight ) > 0.001f )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_slitherPath1( PFMPoint * _minimal, PFMPoint * _p0, TVectorWayPoint & _points )
	{
		_points.push_back( _minimal->v );

		if( _minimal == _p0 )
		{
			return true;
		}

		PFMPoint * next = _minimal;

		for( PFMPoint::TVectorNeighbor::iterator
			it = _minimal->neighbor_other.begin(),
			it_end = _minimal->neighbor_other.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			if( s_slitherPath2( _minimal, neighbor ) == false )
			{
				continue;
			}

			next = neighbor;
			break;
		}

		if( _minimal->neighbor_right != nullptr && s_slitherPath2( _minimal, _minimal->neighbor_left ) == true )
		{
			next = _minimal->neighbor_left;
		}
		else if( _minimal->neighbor_right != nullptr && s_slitherPath2( _minimal, _minimal->neighbor_right ) == true )
		{
			next = _minimal->neighbor_right;
		} 
		else if( next == _minimal )
		{
			return false;
		}

		bool result = s_slitherPath1( next, _p0, _points );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_intersectsPolygonSegment( const Polygon::ring_type & _ring, const Segment & _segment )
	{
		if( _ring.empty() == true )
		{
			return false;
		}

		Polygon::ring_type::size_type size = _ring.size() - 1;

		for( Polygon::ring_type::size_type i = 0; i != size; ++i )
		{
			const mt::vec2f & v0 = _ring[i + 0];
			const mt::vec2f & v1 = _ring[i + 1];

			Segment sg(v0, v1);

			if( boost::geometry::intersects( sg, _segment ) == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_intersectsPolygonSegment( const Polygon & _polygon, const Segment & _segment )
	{
		const Polygon::inner_container_type & inner_container = _polygon.inners();

		for( Polygon::inner_container_type::const_iterator 
			it = inner_container.begin(),
			it_end = inner_container.end();
		it != it_end;
		++it )
		{
			const Polygon::ring_type & ring = *it;

			if( s_intersectsPolygonSegment( ring, _segment ) == true )
			{
				return true;
			}
		}

		if( inner_container.empty() == false )
		{
			return false;
		}

		const Polygon::ring_type & ring = _polygon.outer();

		if( s_intersectsPolygonSegment( ring, _segment ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const
	{
		Segment sg(_p0, _p1);

		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & p = (*it)->hole;

			if( s_intersectsPolygonSegment( p, sg ) == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testHolesPoint_( const mt::vec2f & _p ) const
	{
		GeometryPoint point(_p.x, _p.y);

		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & p = (*it)->hole;

			bool intersect = boost::geometry::intersects( p, point );

			if( intersect == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_removeNeighbor( PFMPoint * _point )
	{
		for( PFMPoint::TVectorNeighbor::iterator
			it = _point->neighbor_other.begin(),
			it_end = _point->neighbor_other.end();
		it != it_end;
		++it )
		{
			PFMPoint * neighbor = *it;

			PFMPoint::TVectorNeighbor::iterator it_found = 
				std::find( neighbor->neighbor_other.begin(), neighbor->neighbor_other.end(), _point );

			neighbor->neighbor_other.erase( it_found );
		}

		_point->neighbor_other.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::fillObstaclePointWeight_( float _weight ) const
	{
		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			Obstacle * obstacle = *it;

			for( PFMPoints::const_iterator
				it_point = obstacle->points.begin(),
				it_point_end = obstacle->points.end();
			it_point != it_point_end;
			++it_point )
			{
				PFMPoint * points = *it_point;

				points->weight = _weight;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay * PathFinderMap::findPath( const mt::vec2f & _from, const mt::vec2f & _to )
	{
		if( this->testHolesPoint_( _to ) == true )
		{
			return nullptr;
		}

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

		if( m_invalidateMap == true )
		{
			if( this->generateMap() == false )
			{
				return nullptr;
			}
		}

		//BEGIN_WATCHDOG(m_serviceProvider, "findPath");
		
		PFMPoint point_from(_from);
		PFMPoint point_to(_to);

		this->attachNeighbor_( &point_from );
		this->attachNeighbor_( &point_to );
		
		this->fillObstaclePointWeight_( 1024.f * 1024.f );

		point_to.weight = 0.f;
		point_from.weight = 1024.f * 1024.f;
		
		s_wavePathPoint( &point_to );

		TVectorWayPoint wayPoints;
		if( s_slitherPath1( &point_from, &point_to, wayPoints ) == false )
		{
			return nullptr;
		}

		s_removeNeighbor( &point_from );
		s_removeNeighbor( &point_to );
				
		PathFinderWay * way = new PathFinderWay(m_serviceProvider);

		way->initialize( _from, _to, wayPoints );

		m_pathFinderWays.push_back( way );

		//END_WATCHDOG(m_serviceProvider, "findPath", 0)("path find");

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
	void PathFinderMap::renderPolygonRing_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Polygon::ring_type & _ring, uint32_t _color )
	{
		size_t numpoints = _ring.size();

		if( numpoints == 0 )
		{
			return;
		}

		size_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		for( size_t i = 0; i != numpoints; ++i )
		{
			size_t j = (i + 1) % numpoints;

			mt::vec2f trP0 = _ring[i];
			RenderVertex2D & v0 = vertices[i*2+0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = _color;
			v0.uv.x = 0.f;
			v0.uv.y = 0.f;
			v0.uv2.x = 0.f;
			v0.uv2.y = 0.f;

			mt::vec2f trP1 = _ring[j];
			RenderVertex2D & v1 = vertices[i*2+1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = _color;
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
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::renderPointsNeighbor_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const PFMPoints & _points, uint32_t _color )
	{
		for( PFMPoints::const_iterator
			it = _points.begin(),
			it_end = _points.end();
		it != it_end;
		++it )
		{
			PFMPoint * p = *it;

			size_t numpoints = p->neighbor_other.size();

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
				it_neighbor_end = p->neighbor_other.size();
			it_neighbor != it_neighbor_end;
			++it_neighbor )
			{		
				PFMPoint * n = p->neighbor_other[it_neighbor];

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

		for( TVectorObstacles::iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & polygon = (*it)->hole;
			
			const Polygon::ring_type & ring = polygon.outer();

			this->renderPolygonRing_( _viewport, _camera, ring, 0xFFFFFF00 );

			const Polygon::inner_container_type & inners = polygon.inners();

			for( Polygon::inner_container_type::const_iterator
				it = inners.begin(),
				it_end = inners.end();
			it != it_end;
			++it )
			{
				const Polygon::ring_type & ring = *it;

				this->renderPolygonRing_( _viewport, _camera, ring, 0xFFFFFF00 );
			}
		}

		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const PFMPoints & points = (*it)->points;

			this->renderPointsNeighbor_( _viewport, _camera, points, 0xFFFF00FF );
		}

		for( TVectorObstacles::iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & polygon = (*it)->bigHole;

			const Polygon::ring_type & ring = polygon.outer();

			this->renderPolygonRing_( _viewport, _camera, ring, 0xFFFF0000 );

			const Polygon::inner_container_type & inners = polygon.inners();

			for( Polygon::inner_container_type::const_iterator
				it_ring = inners.begin(),
				it_ring_end = inners.end();
			it_ring != it_ring_end;
			++it_ring )
			{
				const Polygon::ring_type & ring = *it_ring;

				this->renderPolygonRing_( _viewport, _camera, ring, 0xFFFF0000 );
			}
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