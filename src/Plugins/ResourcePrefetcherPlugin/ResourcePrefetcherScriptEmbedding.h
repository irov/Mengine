#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class ResourcePrefetcherScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( ResourcePrefetcherScriptEmbedding );

    public:
        ResourcePrefetcherScriptEmbedding();
        ~ResourcePrefetcherScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}