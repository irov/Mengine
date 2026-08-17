#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UIFrameworkScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( UIFrameworkScriptEmbedding );

    public:
        UIFrameworkScriptEmbedding();
        ~UIFrameworkScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
    //////////////////////////////////////////////////////////////////////////
}
