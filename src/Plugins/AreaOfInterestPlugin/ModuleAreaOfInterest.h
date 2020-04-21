#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class ModuleAreaOfInterest
        : public ModuleBase
    {
    public:
        ModuleAreaOfInterest();
        ~ModuleAreaOfInterest() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    public:
        void _update( bool _focus ) override;
    };
}