#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSAdvertisementScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSAdvertisementScriptEmbedding );

    public:
        iOSAdvertisementScriptEmbedding();
        ~iOSAdvertisementScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
