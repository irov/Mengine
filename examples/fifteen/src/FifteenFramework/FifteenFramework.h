#pragma once

#include "Kernel/FrameworkBase.h"

namespace Mengine
{
    class FifteenFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( FifteenFramework );

    public:
        FifteenFramework();
        ~FifteenFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
