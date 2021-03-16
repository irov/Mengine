#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class AstralaxModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AstralaxModule );

    public:
        AstralaxModule();
        ~AstralaxModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;
    };
}