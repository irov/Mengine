#pragma once

#include "Kernel/ThreadTask.h"

#include "Factory/Factorable.h"

#include "fastpathfinder/pathfinder.h"
#include "fastpathfinder/map.h"

#include "math/vec2.h"

#include "pybind/pybind.hpp"

namespace Mengine
{	
	typedef fastpathfinder::pathfinder<fastpathfinder::map_test_wall_none> TPathFinder;
	typedef fastpathfinder::map TPathMap;
	
	class PathFinder
		: public ThreadTask
	{
	public:
		PathFinder();
		~PathFinder();

	public:
		bool initialize( TPathMap * _map, const mt::vec2f & _from, const mt::vec2f & _to, float _gridSize );

	public:
		pybind::list getWay();

	protected:
		bool _onRun() override;
		bool _onMain() override;
		void _onComplete( bool _successful ) override;

	protected:
		mt::vec2f m_from;
		mt::vec2f m_to;

		float m_gridSize;

		TPathFinder m_pathfinder;
		pybind::list m_way;
	};

	typedef stdex::intrusive_ptr<PathFinder> PathFinderPtr;
}
