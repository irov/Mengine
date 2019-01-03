#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class EngineScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        EngineScriptEmbedding();
        ~EngineScriptEmbedding() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}