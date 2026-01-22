#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class ImGUIScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( ImGUIScriptEmbedding );

    public:
        ImGUIScriptEmbedding();
        ~ImGUIScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
