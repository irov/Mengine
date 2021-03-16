#pragma once

#include "Kernel/FrameworkBase.h"

namespace Mengine
{
    class UIFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( UIFramework );

    public:
        UIFramework();
        ~UIFramework() override;

    public:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
