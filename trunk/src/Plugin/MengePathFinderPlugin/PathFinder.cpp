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
		if( _from.x < 0.f || _from.y < 0.f )
		{
			return false;
		}

		if( _to.x < 0.f || _to.y < 0.f )
		{
			return false;
		}

		if( m_pathfinder.initialize( _map ) == false )
		{
			return false;
		}

		m_from = _from;
		m_to = _to;
		m_gridSize = _gridSize;

		uint32_t fx = (uint32_t)(_from.x / m_gridSize + 0.5f);
		uint32_t fy = (uint32_t)(_from.y / m_gridSize + 0.5f);

		uint32_t tx = (uint32_t)(_to.x / m_gridSize + 0.5f);
		uint32_t ty = (uint32_t)(_to.y / m_gridSize + 0.5f);

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
	bool PathFinder::update( bool & _found )
	{
		bool found_path;
		if( m_pathfinder.findPathNext( found_path ) == false )
		{
			return false;
		}

		_found = found_path;

		if( found_path == false )
		{
			return true;
		}

		m_pathfinder.findProcces();
		m_pathfinder.findFilter();

		m_way = new PathFinderWay();

		m_way->setServiceProvider( m_serviceProvider );

		const fastpathfinder::point_array & pa = m_pathfinder.getPathFilter();
		m_way->initialize( m_from, m_to, m_gridSize, pa );

		return true;
	}
}