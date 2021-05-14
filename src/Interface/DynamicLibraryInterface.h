#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceProviderInterface.h"
#include "Interface/PluginInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef void * (*TDynamicLibraryFunction)(void *);
    //////////////////////////////////////////////////////////////////////////
    class DynamicLibraryInterface
        : public Interface
    {
    public:
        virtual TDynamicLibraryFunction getSymbol( const Char * _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DynamicLibraryInterface> DynamicLibraryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}