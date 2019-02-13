#pragma once

#include "Interface/Interface.h"
#include "Interface/ModuleInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class ModuleFactoryInterface
        : public Interface
    {
    public:
        virtual ModuleInterfacePtr createModule( const ConstString & _name, const Char * _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleFactoryInterface> ModuleFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}