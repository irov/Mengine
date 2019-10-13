#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class MyGameFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        MyGameFramework();
        ~MyGameFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
