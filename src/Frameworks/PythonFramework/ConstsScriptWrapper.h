#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ConstsScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        ConstsScriptWrapper();
        ~ConstsScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;
    };
}