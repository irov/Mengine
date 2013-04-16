#   pragma once

#   include "Interface/PluginInterface.h"

namespace Menge
{
    class DynamicLibraryInterface;

    class PluginService
        : public PluginServiceInterface
    {
    public:
        PluginService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        PluginInterface * loadPlugin( const String & _name ) override;
        void unloadPlugin( const String & _name ) override;

    public:
        void destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        struct PluginDesc
        {
            DynamicLibraryInterface * dlib;
            PluginInterface * plugin;
        };

        typedef std::map<String, PluginDesc> TMapPlugins;
        TMapPlugins m_plugins;

        String m_dllCreatePluginName;
    };
}