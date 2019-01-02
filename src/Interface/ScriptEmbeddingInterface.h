#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class ScriptEmbeddingInterface
        : public Mixin
    {
    public:
        virtual bool embedding() = 0;
        virtual void ejecting() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptEmbeddingInterface> ScriptEmbeddingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}