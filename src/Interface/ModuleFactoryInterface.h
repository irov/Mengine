#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ModuleInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class ModuleFactoryInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual ModuleInterfacePtr createModule( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleFactoryInterface> ModuleFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}