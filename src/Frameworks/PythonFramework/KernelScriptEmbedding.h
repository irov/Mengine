#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class KernelScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( KernelScriptEmbedding );

    public:
        KernelScriptEmbedding();
        ~KernelScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}