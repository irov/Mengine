#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class EntityScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        EntityScriptWrapper();
        ~EntityScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}