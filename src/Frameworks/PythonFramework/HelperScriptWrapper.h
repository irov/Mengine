#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class HelperScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        HelperScriptWrapper();
        ~HelperScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}