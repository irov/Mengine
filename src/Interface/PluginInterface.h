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

    public:
		virtual void destroy() = 0;
	};

	typedef bool (*TPluginCreate)( PluginInterface ** _plugin );

    class PluginServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PluginService")

    public:
        virtual PluginInterface * loadPlugin( const String & _name ) = 0;
        virtual void unloadPlugin( const String & _name ) = 0;

    public:
        virtual void destroy() = 0;
    };

#   define PLUGIN_SERVICE( serviceProvider )\
    (Menge::getService<Menge::PluginServiceInterface>(serviceProvider))
}