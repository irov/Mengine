#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class TexturepackerScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( TexturepackerScriptEmbedding );

    public:
        TexturepackerScriptEmbedding();
        ~TexturepackerScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}