#pragma once

#include "Interface/PluginInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_map.h"

namespace Mengine
{
#ifndef MENGINE_PLUGIN_NAME_MAX
#	define MENGINE_PLUGIN_NAME_MAX 32
#endif

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
		bool loadPlugin( const WString & _dllName ) override;
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

		typedef stdex::vector<PluginDesc> TVectorPlugins;
		TVectorPlugins m_plugins;
	};
}