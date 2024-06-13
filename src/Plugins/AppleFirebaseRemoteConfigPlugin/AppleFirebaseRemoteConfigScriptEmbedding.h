#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AppleFirebaseRemoteConfigScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
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