#   pragma once

#   include "Interface/PluginInterface.h"

#   include "Core/ServiceBase.h"

#   include <stdex/stl_map.h>

namespace Menge
{
#	ifndef MENGINE_PLUGIN_NAME_MAX
#	define MENGINE_PLUGIN_NAME_MAX 32
#	endif

    class PluginService
        : public ServiceBase<PluginServiceInterface>
    {
    public:
        PluginService();
        ~PluginService();

	public:
		bool _initialize() override;
		void _finalize() override;

    public:
		bool loadPlugin( const WString & _dllName ) override;
		bool createPlugin( DynamicLibraryInterface * _dlib, TPluginCreate _create, bool _dynamic ) override;

	public:
		bool addPlugin( DynamicLibraryInterface * _dlib, PluginInterface * _plugin ) override;
		bool removePlugin( PluginInterface * _plugin ) override;
		bool hasPlugin( const Char * _name ) const override;
		PluginInterface * getPlugin( const Char * _name ) const override;

    protected:
        struct PluginDesc
        {
			Char name[MENGINE_PLUGIN_NAME_MAX];
            DynamicLibraryInterface * dlib;
            PluginInterface * plugin;
        };

		typedef stdex::vector<PluginDesc> TVectorPlugins;
		TVectorPlugins m_plugins;
	};
}