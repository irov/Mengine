#	include "PathFinderMap.h"

#	include "Interface/WatchdogInterface.h"
#	include "Interface/StringizeInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static bool s_polygonToBox( const Polygon & _polygon, mt::vec2f & _min, mt::vec2f & _max )
	{
		if( boost::geometry::num_points(_polygon) == 0 )
		{
			return false;
		}

		const Polygon::ring_type & ring = _polygon.outer();

		const mt::vec2f & v0 = ring[0];
		_min = v0;
		_max = v0;

		Polygon::ring_type::size_type size = ring.size() - 1;

		for( Polygon::ring_type::size_type i = 1; i != size; ++i )
		{
			const mt::vec2f & v = ring[i];

			if( _min.x > v.x )
			{
				_min.x = v.x;
			}

			if( _min.y > v.y )
			{
				_min.y = v.y;
			}

			if( _max.x < v.x )
			{
				_max.x = v.x;
			}

			if( _max.y < v.y )
			{
				_max.y = v.y;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap::PathFinderMap()
		: m_serviceProvider(nullptr)
		, m_width(0)
		, m_height(0)
		, m_gridSize(16.f)
		, m_unitSize(20.f)
		, m_enumeratorObstacles(0)
		, m_enumeratorPathFinders(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap::~PathFinderMap()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::initialize()
	{
		THREAD_SERVICE(m_serviceProvider)
			->createThread( CONST_STRING_LOCAL(m_serviceProvider, "ThreadPathFinderMap"), 0 );

		m_threadPathFinders = THREAD_SERVICE(m_serviceProvider)
			->runTaskQueue( CONST_STRING_LOCAL(m_serviceProvider, "ThreadPathFinderMap"), 1, 1 );

		if( m_threadPathFinders == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setMap( float _width, float _height, float _gridSize, float _unitSize )
	{
		m_width = _width;
		m_height = _height;
		m_gridSize = _gridSize;

		m_unitSize = _unitSize;

		uint16_t map_width = (uint16_t)(_width / _gridSize + 0.5f);
		uint16_t map_height = (uint16_t)(_height / _gridSize + 0.5f);

		m_map.initialize( map_width, map_height );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setMapWeight( const uint8_t * _weight )
	{
		m_map.setCellWeightBuffer( _weight );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testMap( const mt::vec2f & _point ) const
	{
		uint16_t px = (uint16_t)(_point.x / m_gridSize + 0.5f);
		uint16_t py = (uint16_t)(_point.y / m_gridSize + 0.5f);

		uint8_t mask;
		if( m_map.getCellMask( px, py, mask ) == false )
		{
			return false;
		}

		if( mask == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_enlargePolygonRingFromLow( Polygon::ring_type & _ring, float _radius )
	{
		if( _ring.empty() == true )
		{
			return;
		}

		Polygon::ring_type::size_type numpoints = _ring.size();

		--numpoints;

		for( Polygon::ring_type::size_type i = 0; i != numpoints; ++i )
		{
			Polygon::ring_type::size_type i0 = (i + numpoints - 1) % numpoints;
			Polygon::ring_type::size_type i1 = (i + numpoints + 0) % numpoints;
			Polygon::ring_type::size_type i2 = (i + numpoints + 1) % numpoints;

			const mt::vec2f & p0 = _ring[i0];
			const mt::vec2f & p1 = _ring[i1];
			const mt::vec2f & p2 = _ring[i2];

			mt::vec2f v0 = p1 - p0;
			mt::vec2f v0_norm;
			mt::norm_v2_v2( v0_norm, v0 );
			mt::vec2f v0_norm_perp;
			mt::perp_v2( v0_norm_perp, v0_norm );

			mt::vec2f v1 = p2 - p1;
			mt::vec2f v1_norm;
			mt::norm_v2_v2( v1_norm, v1 );
			mt::vec2f v1_norm_perp;
			mt::perp_v2( v1_norm_perp, v1_norm );

			mt::vec2f d = v0_norm_perp + v1_norm_perp;
			mt::vec2f d_norm;
			mt::norm_v2_v2( d_norm, d );

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

		Polygon::ring_type::size_type numpoints = _ring.size();

		--numpoints;

		for( Polygon::ring_type::size_type i = 0; i != numpoints; ++i )
		{
			Polygon::ring_type::size_type i0 = (i + numpoints - 1) % numpoints;
			Polygon::ring_type::size_type i1 = (i + numpoints + 0) % numpoints;
			Polygon::ring_type::size_type i2 = (i + numpoints + 1) % numpoints;
			
			const mt::vec2f & p0 = _ring[i0];
			const mt::vec2f & p1 = _ring[i1];
			const mt::vec2f & p2 = _ring[i2];

			mt::vec2f v0 = p1 - p0;
			mt::vec2f v0_norm;
			mt::norm_v2_v2( v0_norm, v0 );
			mt::vec2f v0_norm_perp;
			mt::perp_v2( v0_norm_perp, v0_norm );

			mt::vec2f v1 = p2 - p1;
			mt::vec2f v1_norm;
			mt::norm_v2_v2( v1_norm, v1 );
			mt::vec2f v1_norm_perp;
			mt::perp_v2( v1_norm_perp, v1_norm );

			mt::vec2f d = v0_norm_perp + v1_norm_perp;
			mt::vec2f d_norm;
			mt::norm_v2_v2( d_norm, d );

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
	bool PathFinderMap::testPoint( const mt::vec2f & _point )
	{
		bool test = this->testHolesPoint_( _point );

		return test;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderMap::testObstacle( const Polygon & _polygon )
	{
		Polygon big_polygon = _polygon;
		boost::geometry::correct( big_polygon );

		s_enlargePolygonFromLow( big_polygon, m_unitSize );

		bool test = this->testBigHolesPolygon_( big_polygon );
		
		return test;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t PathFinderMap::addObstacle( const Polygon & _polygon )
	{	
		if( boost::geometry::num_points(_polygon) == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("PathFinderMap::addObstacle polygon is empty!"
				);

			return 0;
		}

		Polygon big_polygon = _polygon;
		boost::geometry::correct( big_polygon );

		s_enlargePolygonFromLow( big_polygon, m_unitSize );

		uint32_t id = ++m_enumeratorObstacles;

		Obstacle * obstacle = new Obstacle;
		obstacle->id = id;

		obstacle->hole = _polygon;
		boost::geometry::correct( obstacle->hole );

		obstacle->bigHole = big_polygon;
		if( s_polygonToBox( obstacle->bigHole, obstacle->bigMinHole, obstacle->bigMaxHole ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PathFinderMap::addObstacle invalid polygon to box!"
				);

			return 0;
		}

		this->obstacleCellMask_( obstacle, 1 );
				
		m_obstacles.push_back( obstacle );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::removeObstacle( uint32_t _id )
	{
		for( TVectorObstacles::iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			Obstacle * obstacle = (*it);

			if( obstacle->id != _id )
			{
				continue;
			}

			this->obstacleCellMask_( obstacle, 0 );

			m_obstacles.erase( it );

			break;
		}
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
	void PathFinderMap::obstacleCellMask_( Obstacle * _obstacle, uint8_t _mask )
	{
		const Polygon & polygon = _obstacle->bigHole;
		
		const mt::vec2f & minp = _obstacle->bigMinHole;
		const mt::vec2f & maxp = _obstacle->bigMaxHole;

		uint16_t map_width = m_map.getWidth();
		uint16_t map_height = m_map.getHeight();

		uint16_t map_begin_i = (uint16_t)(minp.x / m_gridSize + 0.5f);
		uint16_t map_begin_j = (uint16_t)(minp.y / m_gridSize + 0.5f);

		uint16_t map_end_i = (uint16_t)(maxp.x / m_gridSize + 0.5f);
		uint16_t map_end_j = (uint16_t)(maxp.y / m_gridSize + 0.5f);

		uint16_t map_begin_i_crop = mt::crop( map_begin_i, (uint16_t)0, map_width );
		uint16_t map_begin_j_crop = mt::crop( map_begin_j, (uint16_t)0, map_height );
		uint16_t map_end_i_crop = mt::crop( map_end_i, (uint16_t)0, map_width );
		uint16_t map_end_j_crop = mt::crop( map_end_j, (uint16_t)0, map_height );

		for( uint16_t j = map_begin_j_crop; j != map_end_j_crop; ++j )
		{
			for( uint16_t i = map_begin_i_crop; i != map_end_i_crop; ++i )
			{
				float x = (i * m_gridSize) + m_gridSize * 0.5f;
				float y = (j * m_gridSize) + m_gridSize * 0.5f;

				if( intersection_polygon_point( polygon, x, y ) == false )
				{
					continue;
				}
				
				m_map.setCellMask( i, j, _mask );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_intersectsPolygonRingSegment( const Polygon::ring_type & _ring, const Segment & _segment )
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

			if( s_intersectsPolygonRingSegment( ring, _segment ) == true )
			{
				return true;
			}
		}

		if( inner_container.empty() == false )
		{
			return false;
		}

		const Polygon::ring_type & ring = _polygon.outer();

		if( s_intersectsPolygonRingSegment( ring, _segment ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_intersectsPolygonPoint( const Polygon & _polygon, const mt::vec2f & _point )
	{
		GeometryPoint point(_point.x, _point.y);
		bool intersect = boost::geometry::intersects( _polygon, point );

		return intersect;
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
		for( TVectorObstacles::const_iterator
			it = m_obstacles.begin(),
			it_end = m_obstacles.end();
		it != it_end;
		++it )
		{
			const Polygon & p = (*it)->hole;

			bool intersect = s_intersectsPolygonPoint( p, _p );

			if( intersect == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t PathFinderMap::createPathFinder( const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	{
		if( m_threadPathFinders == nullptr )
		{
			return 0;
		}

		PathFinderPtr pf = m_factoryPathFinder.createObjectT();

		if( pf->initialize( &m_map, _from, _to, m_gridSize ) == false )
		{
			return 0;
		}

		m_threadPathFinders->addTask( pf );

		PathFinderDesc desc;

		uint32_t id = ++m_enumeratorPathFinders;
		desc.id = id;

		desc.finder = pf;
		
		desc.cb = _cb;
		pybind::incref( desc.cb );

		desc.complete = false;
		desc.successful = false;

		m_pathfinders.push_back( desc );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::removePathFinder( uint32_t _finderId )
	{
		for( TVectorPathFinderDesc::iterator
			it = m_pathfinders.begin(),
			it_end = m_pathfinders.end();
		it != it_end;
		++it )
		{
			PathFinderDesc & desc = *it;

			if( desc.id != _finderId )
			{
				continue;
			}

			if( desc.complete == true )
			{
				continue;
			}

			desc.finder->cancel();
			desc.finder = nullptr;
			
			pybind::decref( desc.cb );
			desc.cb = nullptr;

			desc.complete = true;

			m_pathfinders.erase( it );

			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct FPathFinderDead
		{
			bool operator ()( const PathFinderMap::PathFinderDesc & _event ) const
			{
				return _event.complete;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::update()
	{
		TVectorPathFinderDesc pathcomplete;

		for( TVectorPathFinderDesc::iterator
			it = m_pathfinders.begin(),
			it_end = m_pathfinders.end();
		it != it_end;
		++it )
		{
			PathFinderDesc & desc = *it;

			if( desc.finder->isComplete() == false )
			{
				continue;
			}

			if( desc.complete == true )
			{
				continue;
			}
			
			pathcomplete.push_back( desc );
			
			desc.finder = nullptr;
			desc.complete = true;
		}

		TVectorPathFinderDesc::iterator it_erase = std::remove_if( m_pathfinders.begin(), m_pathfinders.end(), FPathFinderDead());
		m_pathfinders.erase( it_erase, m_pathfinders.end() );
		
		for( TVectorPathFinderDesc::iterator
			it = pathcomplete.begin(),
			it_end = pathcomplete.end();
		it != it_end;
		++it )
		{
			PathFinderDesc & desc = *it;

			if( desc.finder->isSuccessful() == false )
			{
				pybind::call( desc.cb, "(IOO)"
					, desc.id
					, pybind::get_bool(false)					
					, pybind::get_none() 
					);
			}
			else
			{
				PyObject * way = desc.finder->getWay();

				pybind::call( desc.cb, "(IOO)"
					, desc.id
					, pybind::get_bool(true)
					, way
					);
			}

			pybind::decref( desc.cb );
			desc.cb = nullptr;
		}

		pathcomplete.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::setCamera2D( const RenderCameraInterface * _camera )
	{
		m_camera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderMap::renderPolygonRing_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Polygon::ring_type & _ring, uint32_t _color )
	{
		Polygon::ring_type::size_type numpoints = _ring.size();

		if( numpoints == 0 )
		{
			return;
		}

		Polygon::ring_type::size_type vertexCount = numpoints * 2;

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		for( Polygon::ring_type::size_type i = 0; i != numpoints; ++i )
		{
			Polygon::ring_type::size_type j = (i + 1) % numpoints;

			mt::vec2f trP0 = _ring[i];
			RenderVertex2D & v0 = vertices[i*2 + 0];

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

		//uint32_t map_width = m_map.getWidth();
		//uint32_t map_height = m_map.getHeight();

		//for( uint32_t j = 0; j != map_height; ++j )
		//{
		//	for( uint32_t i = 0; i != map_width; ++i )
		//	{
		//		uint32_t mask = m_map.getCellMask( i, j );

		//		if( mask == 0 )
		//		{
		//			continue;
		//		}

		//		float x = ( i * m_gridSize ) + m_gridSize * 0.5f;
		//		float y = ( j * m_gridSize ) + m_gridSize * 0.5f;

		//		Polygon polygon;
		//		boost::geometry::append(polygon, mt::vec2f(x + m_gridSize * 0.25f, y - m_gridSize * 0.00f) );
		//		boost::geometry::append(polygon, mt::vec2f(x + m_gridSize * 0.00f, y - m_gridSize * 0.25f) );
		//		boost::geometry::append(polygon, mt::vec2f(x - m_gridSize * 0.25f, y + m_gridSize * 0.00f) );
		//		boost::geometry::append(polygon, mt::vec2f(x - m_gridSize * 0.00f, y + m_gridSize * 0.25f) );
		//		boost::geometry::correct(polygon);

		//		const Polygon::ring_type & ring = polygon.outer();

		//		this->renderPolygonRing_( _viewport, _camera, ring, 0xFF00FF00 );
		//	}
		//}

		//for( TVectorPathFinderWay::iterator
		//	it = m_pathFinderWays.begin(),
		//	it_end = m_pathFinderWays.end();
		//it != it_end;
		//++it )
		//{
		//	PathFinderWay * way = *it;

		//	way->render( _viewport, _camera );
		//}
	}
}