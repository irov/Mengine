#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	typedef std::map<std::string, std::string> TMapParam;

	class PluginInterface
	{
	public:
		virtual void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) = 0;
		virtual void finalize() = 0;
	};

	typedef bool (*TPluginCreate)( PluginInterface ** _plugin );
}