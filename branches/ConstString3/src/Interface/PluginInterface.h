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
	};

	typedef std::map<std::string, std::string> TMapParam;

	class PluginInterface
	{
	public:
		virtual void initialize( ServiceProviderInterface * _provider ) = 0;
		virtual void finalize() = 0;

		virtual void run( const TMapParam & _params ) = 0; 
	};
}