#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class OzzScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        OzzScriptEmbedding();
        ~OzzScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}