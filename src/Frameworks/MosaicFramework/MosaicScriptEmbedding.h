#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class MosaicScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( MosaicScriptEmbedding );

    public:
        MosaicScriptEmbedding();
        ~MosaicScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
