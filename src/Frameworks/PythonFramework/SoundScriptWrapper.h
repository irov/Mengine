#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class SoundScriptWrapper
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        SoundScriptWrapper();
        ~SoundScriptWrapper() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}