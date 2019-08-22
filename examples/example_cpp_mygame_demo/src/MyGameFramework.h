#pragma once

#include "Interface/FrameworkInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class MyGameFramework
        : public ServiceBase<FrameworkInterface>
    {
    public:
        MyGameFramework();
        ~MyGameFramework() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool onFrameworkInitialize() override;
        void onFrameworkFinalize() override;
    };
}
