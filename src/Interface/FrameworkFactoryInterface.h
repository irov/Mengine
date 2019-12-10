#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FrameworkInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class FrameworkFactoryInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual FrameworkInterfacePtr createFramework( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FrameworkFactoryInterface> FrameworkFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}