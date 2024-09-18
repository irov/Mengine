#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/DynamicLibraryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Export.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef bool(*TPluginCreate)(ServiceProviderInterface * _serviceProvider, PluginInterface ** const _plugin, UniqueId _uid, bool _dynamic);
    typedef bool(*TPluginDestroy)(ServiceProviderInterface * _serviceProvider, PluginInterface * _plugin, UniqueId _uid);
    //////////////////////////////////////////////////////////////////////////
    class PluginServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PluginService" )

    public:
        virtual bool loadPlugin( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool createPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, TPluginCreate _create, bool _dynamic, const DocumentInterfacePtr & _doc ) = 0;
        virtual void unloadPlugins() = 0;

    public:
        virtual bool addPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, const PluginInterfacePtr & _plugin ) = 0;
        virtual bool removePlugin( const PluginInterfacePtr & _plugin ) = 0;
        virtual bool hasPlugin( const Char * _name ) const = 0;
        virtual const PluginInterfacePtr & getPlugin( const Char * _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_SERVICE()\
    ((Mengine::PluginServiceInterface *)SERVICE_GET(Mengine::PluginServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_CREATE(Name, Doc)\
    PLUGIN_SERVICE()->createPlugin( nullptr, &PLUGIN_FUNCTION( Name ), false, Doc )
//////////////////////////////////////////////////////////////////////////