#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AttributionScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AttributionScriptEmbedding );

    public:
        AttributionScriptEmbedding();
        ~AttributionScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
