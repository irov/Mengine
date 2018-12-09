#pragma once

#include "Interface/ModuleInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class ModuleFactoryInterface
        : public Mixin
    {
    public:
        virtual ModuleInterfacePtr createModule( const ConstString & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleFactoryInterface> ModuleFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}