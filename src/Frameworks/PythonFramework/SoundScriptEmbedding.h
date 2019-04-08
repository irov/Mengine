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
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}