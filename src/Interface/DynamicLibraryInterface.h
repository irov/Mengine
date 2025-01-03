#pragma once

#include "Interface/ServiceProviderInterface.h"
#include "Interface/PluginInterface.h"

#include "Kernel/Mixin.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef void * (*TDynamicLibraryFunction)(void *);
    //////////////////////////////////////////////////////////////////////////
    class DynamicLibraryInterface
        : public Mixin
    {
    public:
        virtual TDynamicLibraryFunction getSymbol( const Char * _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DynamicLibraryInterface> DynamicLibraryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}