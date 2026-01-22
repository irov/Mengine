#pragma once

#include "Interface/PluginServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class PluginService
        : public ServiceBase<PluginServiceInterface>
    {
        DECLARE_FACTORABLE( PluginService );

    public:
        PluginService();
        ~PluginService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadPlugin( const Char * _dynamicLibraryName, bool _frameworkMode, const DocumentInterfacePtr & _doc ) override;
        bool createPlugin( const DynamicLibraryInterfacePtr & _dynamicLibrary, TPluginCreate _create, bool _dynamicLoad, bool _frameworkMode, const DocumentInterfacePtr & _doc ) override;
        void unloadPlugins() override;

    public:
        bool addPlugin( const PluginInterfacePtr & _plugin, const DynamicLibraryInterfacePtr & _dynamicLibrary ) override;
        bool removePlugin( const PluginInterfacePtr & _plugin ) override;
        bool hasPlugin( const ConstString & _pluginName ) const override;
        const PluginInterfacePtr & getPlugin( const ConstString & _pluginName ) const override;

    public:
        void setAvailablePlugin( const ConstString & _pluginName, bool _available ) override;
        bool isAvailablePlugin( const ConstString & _pluginName ) const override;

    protected:
        bool autoRegistration_( const PluginInterfacePtr & _plugin );

    protected:
        struct PluginDesc
        {
            DynamicLibraryInterfacePtr dynamicLibrary;
            PluginInterfacePtr plugin;
        };

        typedef Vector<PluginDesc> VectorPlugins;
        VectorPlugins m_plugins;

        ThreadMutexInterfacePtr m_pluginsMutex;

        typedef Map<ConstString, bool> MapAvailablePlugins;
        MapAvailablePlugins m_availablePlugins;
    };
}