#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class TrafficJamFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( TrafficJamFramework );

    public:
        TrafficJamFramework();
        ~TrafficJamFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
