#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class SoundScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        SoundScriptEmbedding();
        ~SoundScriptEmbedding() override;

    public:
        bool embedding() override;
        void ejecting() override;

    protected:
        FactorablePtr m_implement;
    };
}