#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceProviderInterface.h"
#include "Interface/PluginInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef bool(*TPluginCreate)(ServiceProviderInterface * _serviceProvider, PluginInterface ** _plugin, bool _dynamic);
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