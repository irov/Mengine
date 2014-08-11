#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "AreaOfInterest.h"

#	include "fastpathfinder/graph.h"

namespace Menge
{
	class ModuleAreaOfInterest
		: public ModuleInterface
	{
	public:
		ModuleAreaOfInterest();
		~ModuleAreaOfInterest();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	public:
		AreaOfInterest * createAOI();
		void removeAOI( AreaOfInterest * _aoi );

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef std::vector<AreaOfInterest *> TVectorAreaOfInterest;
		TVectorAreaOfInterest m_aois;
	};
}