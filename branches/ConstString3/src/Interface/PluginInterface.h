#	pragma once

#	include "Config/Typedef.h"

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

	class PluginInterface
	{
	public:
		virtual void initialize( ServiceProviderInterface * _provider ) = 0;
	};
}

typedef bool (*initPlugin)( Menge::PluginInterface** );
typedef void (*releasePlugin)( Menge::PluginInterface* );
