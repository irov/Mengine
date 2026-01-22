#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class NodeScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( NodeScriptEmbedding );

    public:
        NodeScriptEmbedding();
        ~NodeScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}