#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderMap.h"
#	include "PathFinderWayAffector.h"

#	include "Kernel/Node.h"

#	include "fastpathfinder/graph.h"

namespace Menge
{
	class ModulePathFinder
		: public ModuleInterface
	{
	public:
		ModulePathFinder();
		~ModulePathFinder();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	public:
		PathFinderMap * createMap();
		void destroyMap( PathFinderMap * _map );

		fastpathfinder::graph * createGraph();
		void destroyGraph( fastpathfinder::graph * _graph );

	public:
		bool setMapWeight( PathFinderMap * _map, const ConstString & _resourceName );

	public:
		PathFinderWayAffector * createPathFinderWayAffertor( Node * _node, const pybind::list & _way, float _speed, const pybind::object & _cb );
		void destroyPathFinderWayAffertor( PathFinderWayAffector * _affector );

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, unsigned int _debugMask ) override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef stdex::vector<PathFinderMap *> TPathFinderMaps;
		TPathFinderMaps m_maps;

		typedef FactoryPoolStore<PathFinderWayAffector, 16> TFactoryPoolPathFinderWayAffector;
		TFactoryPoolPathFinderWayAffector m_factoryPathFinderWayAffector;
	};
}