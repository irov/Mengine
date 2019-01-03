#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class GameScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        GameScriptWrapper();
        ~GameScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}