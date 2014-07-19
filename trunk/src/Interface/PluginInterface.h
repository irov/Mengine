#	pragma once

#	include "Interface/ServiceInterface.h"

#   include "Config/String.h"

namespace Menge
{
	class PluginInterface
	{
	public:
		PluginInterface(){};
		virtual ~PluginInterface(){};

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
        virtual PluginInterface * loadPlugin( const WString & _name ) = 0;
        virtual void unloadPlugin( const WString & _name ) = 0;
    };

#   define PLUGIN_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::PluginServiceInterface)
}