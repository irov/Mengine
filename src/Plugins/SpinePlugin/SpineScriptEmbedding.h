#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class SpineScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( SpineScriptEmbedding );

    public:
        SpineScriptEmbedding();
        ~SpineScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}