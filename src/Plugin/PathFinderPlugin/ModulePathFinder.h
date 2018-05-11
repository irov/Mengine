#pragma once

#include "Core/ModuleBase.h"

#include "PathFinderMap.h"
#include "PathFinderWayAffector.h"

#include "Kernel/Node.h"

#include "fastpathfinder/graph.h"

namespace Mengine
{
	class ModulePathFinder
		: public ModuleBase
	{
	public:
		ModulePathFinder();
		~ModulePathFinder() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		PathFinderMap * createMap();
		void destroyMap( PathFinderMap * _map );

		fastpathfinder::graph * createGraph();
		void destroyGraph( fastpathfinder::graph * _graph );

	public:
		bool setMapWeight( PathFinderMap * _map, const ConstString & _resourceName );
		
	public:
		PathFinderWayAffectorPtr createPathFinderWayAffector( Node * _node, const pybind::list & _satellite, const pybind::list & _way, float _offset, float _speed, bool _preparePosition, const pybind::object & _cb, const pybind::args & _args );
		void destroyPathFinderWayAffector( const PathFinderWayAffectorPtr & _affector );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderState * _state, uint32_t _debugMask ) override;
		
	protected:
		typedef stdex::vector<PathFinderMap *> TPathFinderMaps;
		TPathFinderMaps m_maps;

		FactoryPtr m_factoryPathFinderWayAffector;
	};
}