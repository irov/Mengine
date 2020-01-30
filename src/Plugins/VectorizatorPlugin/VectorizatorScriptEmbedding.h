#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class VectorizatorScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        VectorizatorScriptEmbedding();
        ~VectorizatorScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}