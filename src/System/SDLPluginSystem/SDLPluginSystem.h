#   pragma once

#   include "Interface/PluginInterface.h"

namespace Menge
{
    class SDLPluginSystem
        : public ServiceBase<PluginSystemInterface>
    {
    public:
        SDLPluginSystem();
        ~SDLPluginSystem();

    public:
        DynamicLibraryInterface * loadDynamicLibrary( const WString & _dynamicLibraryName ) override;
    };
}
