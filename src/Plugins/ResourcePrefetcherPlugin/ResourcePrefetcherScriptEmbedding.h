#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class ResourcePrefetcherScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        ResourcePrefetcherScriptEmbedding();
        ~ResourcePrefetcherScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}