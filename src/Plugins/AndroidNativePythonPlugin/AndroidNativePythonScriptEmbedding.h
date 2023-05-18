#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AndroidNativePythonScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        AndroidNativePythonScriptEmbedding();
        ~AndroidNativePythonScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}