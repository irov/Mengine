#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSFirebaseRemoteConfigScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSFirebaseRemoteConfigScriptEmbedding );

    public:
        iOSFirebaseRemoteConfigScriptEmbedding();
        ~iOSFirebaseRemoteConfigScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}
