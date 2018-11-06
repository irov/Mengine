#pragma once

#include "Kernel/ModuleBase.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    class AstralaxParticleModule
        : public ModuleBase
    {
    public:
        AstralaxParticleModule();
        ~AstralaxParticleModule() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;

    protected:
        VisitorPtr m_particleValidateVisitor;
    };
}