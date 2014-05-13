#	include "PathFinder.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	PathFinder::PathFinder()
		: m_serviceProvider(nullptr)
		, m_from(0.f, 0.f)
		, m_to(0.f, 0.f)
		, m_way(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinder::~PathFinder()
	{
		delete m_way;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinder::initialize( TPathMap * _map, const mt::vec2f & _from, const mt::vec2f & _to, float _gridSize )
	{
		if( m_pathfinder.initialize( _map ) == false )
		{
			return false;
		}

		m_from = _from;
		m_to = _to;
		m_gridSize = _gridSize;

		uint32_t fx = _from.x / m_gridSize;
		uint32_t fy = _from.y / m_gridSize;

		uint32_t tx = _to.x / m_gridSize;
		uint32_t ty = _to.y / m_gridSize;

		if( m_pathfinder.findPathFirst( fx, fy, tx, ty ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay * PathFinder::getWay() const
	{
		return m_way;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinder::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinder::_onMain()
	{
		bool found_path;
		while( m_pathfinder.findPathNext( found_path ) == false )
		{
		}

		if( found_path == false )
		{
			return false;
		}

		m_pathfinder.findProcces();
		m_pathfinder.findFilter();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinder::_onComplete( bool _successful )
	{
		if( _successful == false )
		{
			return;
		}

		const fastpathfinder::point_array & pa = m_pathfinder.getPathFilter();
		//const fastpathfinder::point_array & pa = m_map.getPath();
		size_t points_size = pa.size();
		fastpathfinder::point * points = pa.buffer();
		
		m_way = new PathFinderWay();

		m_way->setServiceProvider( m_serviceProvider );
		m_way->initialize( m_from, m_to, points_size );

		for( size_t i = 0; i != points_size; ++i )
		{
			fastpathfinder::point p = points[i];

			float x = p.x * m_gridSize + m_gridSize * 0.5f;
			float y = p.y * m_gridSize + m_gridSize * 0.5f;

			m_way->addPoint( mt::vec2f(x, y) );
		}
	}
}