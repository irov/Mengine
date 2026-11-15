#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class Win32NativePythonScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( Win32NativePythonScriptEmbedding );

    public:
        Win32NativePythonScriptEmbedding();
        ~Win32NativePythonScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
