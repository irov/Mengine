#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class Win32OpButtonScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( Win32OpButtonScriptEmbedding );

    public:
        Win32OpButtonScriptEmbedding();
        ~Win32OpButtonScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
