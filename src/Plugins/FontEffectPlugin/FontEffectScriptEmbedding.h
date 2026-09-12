#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class FontEffectScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( FontEffectScriptEmbedding );

    public:
        FontEffectScriptEmbedding();
        ~FontEffectScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}
