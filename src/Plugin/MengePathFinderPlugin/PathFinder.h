#	pragma once

#	include "PathFinderWay.h"

#	include "fastpathfinder/pathfinder.h"
#	include "fastpathfinder/map.h"

#	include "Factory/FactorablePtr.h"

#	include "pybind/pybind.hpp"

namespace Menge
{	
	typedef fastpathfinder::pathfinder<fastpathfinder::map_test_wall_none> TPathFinder;
	typedef fastpathfinder::map TPathMap;

	class PathFinderMap;

	class PathFinder
		: public FactorablePtr
	{
	public:
		void initialize( PathFinderMap * _map );

	public:
		PathFinderWay * findPath( const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb );

	protected:
		PathFinderMap * m_map;

		TPathFinder m_pathfinder;
	};

	typedef stdex::intrusive_ptr<PathFinder> PathFinderPtr;
}