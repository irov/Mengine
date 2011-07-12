#	pragma once

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class ServiceInterface
	{
	};

	class ServiceProviderInterface
	{
	public:
		virtual ServiceInterface * getService( const String & _name ) = 0;

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