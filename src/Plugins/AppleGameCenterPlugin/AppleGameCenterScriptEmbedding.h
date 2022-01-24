#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AppleGameCenterScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        AppleGameCenterScriptEmbedding();
        ~AppleGameCenterScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}