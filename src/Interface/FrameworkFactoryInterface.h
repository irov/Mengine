#pragma once

#include "Interface/Interface.h"
#include "Interface/FrameworkInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class FrameworkFactoryInterface
        : public Interface
    {
    public:
        virtual FrameworkInterfacePtr createFramework( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FrameworkFactoryInterface> FrameworkFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}