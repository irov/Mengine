#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class VideoScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        VideoScriptEmbedding();
        ~VideoScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}