#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSNativePythonScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSNativePythonScriptEmbedding );

    public:
        iOSNativePythonScriptEmbedding();
        ~iOSNativePythonScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
