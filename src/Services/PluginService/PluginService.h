#pragma once

#include "Interface/PluginServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

namespace Mengine
{
    class PluginService
        : public ServiceBase<PluginServiceInterface>
    {
    public:
        PluginService();
        ~PluginService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadPlugin( const Char * _dynamicLibraryName ) override;
        bool createPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, TPluginCreate _create, bool _dynamic ) override;
        void unloadPlugins() override;

    public:
        bool addPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, const PluginInterfacePtr & _plugin ) override;
        bool removePlugin( const PluginInterfacePtr & _plugin ) override;
        bool hasPlugin( const Char * _name ) const override;
        const PluginInterfacePtr & getPlugin( const Char * _name ) const override;

    protected:
        struct PluginDesc
        {
            Char name[MENGINE_PLUGIN_NAME_MAX];
            DynamicLibraryInterfacePtr dynamicLibrary;
            PluginInterfacePtr plugin;
        };

        typedef Vector<PluginDesc> VectorPlugins;
        VectorPlugins m_plugins;
    };
}