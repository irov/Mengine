#pragma once

#include "Interface/Interface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CodecFactoryInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CodecFactoryInterface> CodecFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

