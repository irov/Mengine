#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class BubblegumFramework
        : public FrameworkBase
    {
    public:
        BubblegumFramework();
        ~BubblegumFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
