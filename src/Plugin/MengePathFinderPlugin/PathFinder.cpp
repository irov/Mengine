#	include "PathFinder.h"

#	include "Logger/Logger.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	PathFinder::PathFinder()
		: m_serviceProvider(nullptr)
		, m_from(0.f, 0.f)
		, m_to(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinder::~PathFinder()
	{
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

		uint16_t fx = (uint16_t)(_from.x / m_gridSize + 0.5f);
		uint16_t fy = (uint16_t)(_from.y / m_gridSize + 0.5f);

		uint16_t tx = (uint16_t)(_to.x / m_gridSize + 0.5f);
		uint16_t ty = (uint16_t)(_to.y / m_gridSize + 0.5f);

		if( m_pathfinder.findPathFirst( fx, fy, tx, ty ) == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::list PathFinder::getWay()
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
		bool found_path = false;
		while( m_pathfinder.walkerWave( found_path ) == false )
		{
		}

		if( found_path == false )
		{
			return false;
		}

		if( m_pathfinder.findPath() == false )
		{
			return false;
		}

		//if( m_pathfinder.filterPath() == false )
		//{
		//	return false;
		//}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinder::_onComplete( bool _successful )
	{
		if( _successful == false )
		{
			return;
		}
		
		const fastpathfinder::point_array & pa = m_pathfinder.getPath();

		fastpathfinder::point_array::size_type pa_size = pa.size();

		pybind::list py_way( pa_size );

		py_way[0] = m_from;

		for( fastpathfinder::point_array::size_type i = 1; i != (pa_size - 1); ++i )
		{
			fastpathfinder::point p = pa[i];

			mt::vec2f v;
			v.x = p.x * m_gridSize + m_gridSize * 0.5f;
			v.y = p.y * m_gridSize + m_gridSize * 0.5f;

			py_way[i] = v;
		}

		py_way[pa_size - 1] = m_to;

		m_way = py_way;
	}
}