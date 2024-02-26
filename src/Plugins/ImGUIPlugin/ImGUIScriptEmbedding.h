#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ImGUIScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        ImGUIScriptEmbedding();
        ~ImGUIScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}