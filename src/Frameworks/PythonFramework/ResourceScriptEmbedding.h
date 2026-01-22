#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( ResourceScriptEmbedding );

    public:
        ResourceScriptEmbedding();
        ~ResourceScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}