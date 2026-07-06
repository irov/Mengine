#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSAppTrackingScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSAppTrackingScriptEmbedding );

    public:
        iOSAppTrackingScriptEmbedding();
        ~iOSAppTrackingScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
