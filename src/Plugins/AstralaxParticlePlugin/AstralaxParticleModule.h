#pragma once

#include "Kernel/ModuleBase.h"

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
    };
}