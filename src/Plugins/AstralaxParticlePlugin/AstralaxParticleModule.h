#pragma once

#include "Kernel/ModuleBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class AstralaxParticleModule
        : public ModuleBase
        , public Observable
    {
    public:
        AstralaxParticleModule();
        ~AstralaxParticleModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;
    };
}