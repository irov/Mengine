#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/String.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( String * const )> LambdaTextArgumentContext;
    //////////////////////////////////////////////////////////////////////////
    class TextArgumentInterface
        : public ServantInterface
    {
    public:
        virtual const String & getValue() const = 0;

    public:
        virtual bool updateContext() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextArgumentInterface> TextArgumentInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
