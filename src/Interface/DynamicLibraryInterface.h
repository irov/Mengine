#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceProviderInterface.h"
#include "Interface/PluginInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef bool(*TPluginCreate)(ServiceProviderInterface * _serviceProvider, PluginInterface ** const _plugin, uint32_t _uid, bool _dynamic);
    //////////////////////////////////////////////////////////////////////////
    typedef void * (*TDynamicLibraryFunction)(void *);
    //////////////////////////////////////////////////////////////////////////
    class DynamicLibraryInterface
        : public Interface
    {
    public:
        virtual bool load() = 0;

    public:
        virtual TDynamicLibraryFunction getSymbol( const Char * _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DynamicLibraryInterface> DynamicLibraryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}