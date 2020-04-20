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
        bool _initializeModule() override;
        void _finalizeModule() override;
    };
}