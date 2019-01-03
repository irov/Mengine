#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class EntityScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        EntityScriptEmbedding();
        ~EntityScriptEmbedding() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}