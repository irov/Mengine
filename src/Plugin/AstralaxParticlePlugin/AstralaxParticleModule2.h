#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class AstralaxParticleModule2
        : public ModuleBase
    {
    public:
        AstralaxParticleModule2();
        ~AstralaxParticleModule2() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}