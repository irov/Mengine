#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class HelperScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        HelperScriptEmbedding();
        ~HelperScriptEmbedding() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}