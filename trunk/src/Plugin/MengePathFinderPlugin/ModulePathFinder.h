#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderMap.h"

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

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef std::vector<PathFinderMap *> TPathFinderMaps;
		TPathFinderMaps m_maps;
	};
}