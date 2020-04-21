#pragma once

#include "Kernel/FrameworkBase.h"

namespace Mengine
{
    class UIFramework
        : public FrameworkBase
    {
    public:
        UIFramework();
        ~UIFramework() override;

    public:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
