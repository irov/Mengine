#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class NodeScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        NodeScriptEmbedding();
        ~NodeScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}