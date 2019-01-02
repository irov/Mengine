#pragma once

#include "Interface/PluginInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

#ifndef MENGINE_PLUGIN_NAME_MAX
#define MENGINE_PLUGIN_NAME_MAX 64
#endif

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
        bool loadPlugin( const Char * _dllName ) override;
        bool createPlugin( const DynamicLibraryInterfacePtr & _dlib, TPluginCreate _create, bool _dynamic ) override;

    public:
        bool addPlugin( const DynamicLibraryInterfacePtr & _dlib, const PluginInterfacePtr & _plugin ) override;
        bool removePlugin( const PluginInterfacePtr & _plugin ) override;
        bool hasPlugin( const Char * _name ) const override;
        const PluginInterfacePtr & getPlugin( const Char * _name ) const override;

    protected:
        struct PluginDesc
        {
            Char name[MENGINE_PLUGIN_NAME_MAX];
            DynamicLibraryInterfacePtr dlib;
            PluginInterfacePtr plugin;
        };

        typedef Vector<PluginDesc> VectorPlugins;
        VectorPlugins m_plugins;
    };
}