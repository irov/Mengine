#	pragma once

#	include "Core/ModuleBase.h"

#	include "PathFinderMap.h"
#	include "PathFinderWayAffector.h"

#	include "Kernel/Node.h"

#	include "fastpathfinder/graph.h"

namespace Menge
{
	class ModulePathFinder
		: public ModuleBase
	{
	public:
		ModulePathFinder();
		~ModulePathFinder();

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
		PathFinderWayAffector * createPathFinderWayAffector( Node * _node, const pybind::list & _satellite, const pybind::list & _way, float _offset, float _speed, bool _preparePosition, const pybind::object & _cb, const pybind::detail::args_operator_t & _args );
		void destroyPathFinderWayAffector( PathFinderWayAffector * _affector );

	public:
		void _update( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, unsigned int _debugMask ) override;
		
	protected:
		typedef stdex::vector<PathFinderMap *> TPathFinderMaps;
		TPathFinderMaps m_maps;

		typedef FactoryPoolStore<PathFinderWayAffector, 16> TFactoryPoolPathFinderWayAffector;
		TFactoryPoolPathFinderWayAffector m_factoryPathFinderWayAffector;
	};
}