#   pragma once

#   include "Interface/PluginInterface.h"

#   include <stdex/stl_map.h>

namespace Menge
{
    class DynamicLibraryInterface;

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
        PluginInterface * loadPlugin( const WString & _name ) override;		
        void unloadPlugin( const WString & _name ) override;

    protected:
        struct PluginDesc
        {
            DynamicLibraryInterface * dlib;
            PluginInterface * plugin;
        };

        typedef stdex::map<WString, PluginDesc> TMapPlugins;
        TMapPlugins m_plugins;

        String m_dllCreatePluginName;
    };
}