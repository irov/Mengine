#   pragma once

#   include "Interface/PluginInterface.h"

namespace Menge
{
    class SDLDynamicLibrary 
        : public DynamicLibraryInterface
    {
    public:
        SDLDynamicLibrary( ServiceProviderInterface * _serviceProvider, const String & _name );
        ~SDLDynamicLibrary();

    public:
        bool load() override;
        void destroy() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    private:
        ServiceProviderInterface * m_serviceProvider;

        String m_name;
        void* m_instance;
    };
};
