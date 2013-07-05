#   pragma once

#   include "Interface/PluginInterface.h"

#   include <map>

namespace Menge
{
    class DynamicLibraryInterface;

    class PluginService
        : public PluginServiceInterface
    {
    public:
        PluginService();
        ~PluginService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        PluginInterface * loadPlugin( const WString & _name ) override;
        void unloadPlugin( const WString & _name ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        struct PluginDesc
        {
            DynamicLibraryInterface * dlib;
            PluginInterface * plugin;
        };

        typedef std::map<WString, PluginDesc> TMapPlugins;
        TMapPlugins m_plugins;

        String m_dllCreatePluginName;
    };
}