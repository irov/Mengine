#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class FrameworkFactoryInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual FrameworkInterfacePtr createFramework( const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FrameworkFactoryInterface> FrameworkFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}