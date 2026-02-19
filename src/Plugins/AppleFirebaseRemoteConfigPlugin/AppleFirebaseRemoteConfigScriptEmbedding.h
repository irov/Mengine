#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AppleFirebaseRemoteConfigScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AppleFirebaseRemoteConfigScriptEmbedding );
        
    public:
        AppleFirebaseRemoteConfigScriptEmbedding();
        ~AppleFirebaseRemoteConfigScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}
