#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class EngineScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        EngineScriptEmbedding();
        ~EngineScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}