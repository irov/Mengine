#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MathScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        MathScriptEmbedding();
        ~MathScriptEmbedding() override;

    public:
        bool embedding() override;
        void ejecting() override;
    };
}