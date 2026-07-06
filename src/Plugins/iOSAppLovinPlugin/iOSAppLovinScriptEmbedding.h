#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSAppLovinScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSAppLovinScriptEmbedding );

    public:
        iOSAppLovinScriptEmbedding();
        ~iOSAppLovinScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
