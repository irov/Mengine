#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderMap.h"

#	include "poly2tri.h"

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

	public:
		PathFinderMap * createMap();

	public:
		void update( float _timing ) override;
		void render() override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<PathFinderMap *> TPathFinderMaps;
		TPathFinderMaps m_maps;
	};
}