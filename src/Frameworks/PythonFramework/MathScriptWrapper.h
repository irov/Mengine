#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MathScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        MathScriptWrapper();
        ~MathScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;
    };
}