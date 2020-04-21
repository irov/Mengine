#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class AstralaxModule
        : public ModuleBase
    {
    public:
        AstralaxModule();
        ~AstralaxModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;
    };
}