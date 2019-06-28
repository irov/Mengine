#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class TexturepackerScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        TexturepackerScriptEmbedding();
        ~TexturepackerScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}