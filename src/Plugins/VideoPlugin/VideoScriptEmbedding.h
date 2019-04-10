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
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}