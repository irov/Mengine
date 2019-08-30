#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class UIFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        UIFramework();
        ~UIFramework() override;

    public:
        bool _initializeFramework() override;
        void _finalizeFramework() override;

    protected:
        void notifyBootstrapperInitializeGame();
    };
}
