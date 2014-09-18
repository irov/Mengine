#	include "PathFinder.h"

#	include "Logger/Logger.h"

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
		pybind::decref( m_way );
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
	PyObject * PathFinder::getWay()
	{
		pybind::incref( m_way );

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

		size_t pa_size = pa.size();

		PyObject * py_way = pybind::list_new( pa_size );

		PyObject * py_from = pybind::ptr( m_from );
		pybind::list_setitem( py_way, 0, py_from );

		for( size_t i = 1; i != (pa_size - 1); ++i )
		{
			fastpathfinder::point p = pa[i];

			mt::vec2f v;
			v.x = p.x * m_gridSize + m_gridSize * 0.5f;
			v.y = p.y * m_gridSize + m_gridSize * 0.5f;

			PyObject * py_v = pybind::ptr( v );
			pybind::list_setitem( py_way, i, py_v );
		}

		PyObject * py_to = pybind::ptr( m_to );
		pybind::list_setitem( py_way, pa_size - 1, py_to );

		m_way = py_way;
	}
}