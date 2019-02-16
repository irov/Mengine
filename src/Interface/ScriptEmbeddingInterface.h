#pragma once

#include "Interface/Interface.h"

namespace Mengine
{
    class ScriptEmbeddingInterface
        : public Interface
    {
    public:
        virtual bool embedding() = 0;
        virtual void ejecting() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptEmbeddingInterface> ScriptEmbeddingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}