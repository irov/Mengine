#	pragma once

#	include "ServiceInterface.h"

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class ServiceProviderInterface
	{
	public:
		virtual ServiceInterface * getService( const String & _name ) = 0;
		virtual void registryService( const String & _name, ServiceInterface * _service ) = 0;
		virtual void unregistryService( const String & _name ) = 0;

	public:
		template<class T>
		T * getServiceT( const String & _name )
		{
			return static_cast<T*>( this->getService(_name) );
		}
	};

	typedef std::map<std::string, std::string> TMapParam;

	class PluginInterface
	{
	public:
		virtual void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) = 0;
		virtual void finalize() = 0;
	};

	typedef bool (*TPluginCreate)( PluginInterface ** _plugin );
}