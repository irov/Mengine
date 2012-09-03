#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class PluginInterface
	{
	public:
		virtual bool initialize( ServiceProviderInterface * _provider ) = 0;
		virtual void finalize() = 0;
	};

	typedef bool (*TPluginCreate)( PluginInterface ** _plugin );
}