#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class ServiceInterface
	{
	};

	class ServiceProviderInterface;

	class ServiceListenerInterface
	{
	public:
		virtual void onRegistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
	};

	class ServiceProviderInterface
	{
	public:
		virtual ServiceInterface * getService( const String & _name ) = 0;
		virtual void registryService( const String & _name, ServiceInterface * _service ) = 0;
		virtual void unregistryService( const String & _name ) = 0;

	public:
		virtual void addServiceListener( const String & _name, ServiceListenerInterface * _serviceListener ) = 0;
		virtual bool removeServiceListener( const String & _name, ServiceListenerInterface * _serviceListener ) = 0;

	public:
		template<class T>
		T * getServiceT( const String & _name )
		{
			return static_cast<T*>( this->getService(_name) );
		}
	};
}