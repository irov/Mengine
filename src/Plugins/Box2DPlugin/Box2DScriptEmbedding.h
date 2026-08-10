#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class Box2DScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( Box2DScriptEmbedding );

    public:
        Box2DScriptEmbedding();
        ~Box2DScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
