#pragma once

#include "Interface/Interface.h"
#include "Interface/ModuleInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class ModuleFactoryInterface
        : public Interface
    {
    public:
        virtual ModuleInterfacePtr createModule( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleFactoryInterface> ModuleFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}