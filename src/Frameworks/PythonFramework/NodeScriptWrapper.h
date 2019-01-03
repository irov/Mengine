#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class NodeScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        NodeScriptWrapper();
        ~NodeScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}