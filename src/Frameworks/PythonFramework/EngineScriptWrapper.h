#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class EngineScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        EngineScriptWrapper();
        ~EngineScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}